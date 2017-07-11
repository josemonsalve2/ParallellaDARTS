#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "e_darts_mutex.h"

#define SAFE_SUM_RESULT 0x2228
#define UNSAFE_SUM_RESULT 0x222C
#define MUTEX_LOCATION 0x2000
#define INIT_SIGNAL 0x2004
#define FINAL_BARRIER 0x2008
#define FINAL_TIMER_RESULT 0x200C
#define SAFE_OP(op, mutex) {            \
            darts_mutex_lock(mutex);    \
            op;                         \
            darts_mutex_unlock(mutex);  \
        }


//function to execute
void sum(unsigned int * value)
{
    (*value) ++;
}
    unsigned initialTimerValue = 0xffffffff;
    unsigned configReg = 0, countResult;
    unsigned useless=2;





void startTimer()
{
    unsigned initialTimerValue = 0xffffffff;
    unsigned configReg = 0;
    asm volatile ("movfs %[configValue],config"
        : [configValue] "=r" (configReg)
        : // No inputs
        );
    configReg = configReg & 0xffffff0f;// Set timer to off
    asm("movts config, %[configValue]"
        : // No outputs
        : [configValue] "r" (configReg)
        );
    // Init timer with 0xffffffff since it decrements
    asm("movts ctimer0, %[value]"
        : // No outputs
        :[value] "ir" (initialTimerValue));
    // Start timer
    configReg = configReg | 0x00000010;// Set timer to count clock
    asm("movts config, %[configValue]"
        : // No outputs
        : [configValue] "r" (configReg)
        );
}

void stopTimer()
{
    unsigned countResult;
    unsigned configReg = 0;
    unsigned * value = (unsigned *) (FINAL_TIMER_RESULT);
    // Stop the timer and get value
    configReg = configReg & 0xffffff0f;// Set timer to count clock
    asm("movfs %[configValue],config\n\t"
        "movfs %[timerValue],ctimer0"
        : [configValue] "=r" (configReg), [timerValue] "=r" (countResult)
        : // No inputs
        );
    *value = (0xffffffff - countResult);
}

int main(void)
{
    // for loops
    int i;

    unsigned base0_0Address = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
    unsigned *initSignal = (unsigned *) (base0_0Address + INIT_SIGNAL);
    unsigned * finalBarrier = (unsigned*) (base0_0Address + FINAL_BARRIER);

    // The mutex
    darts_mutex_t *mutex = (unsigned *) (base0_0Address + MUTEX_LOCATION);

    // Sum values
    unsigned * safeSum = (unsigned*) (base0_0Address + SAFE_SUM_RESULT);
    unsigned * unsafeSum = (unsigned*) (base0_0Address + UNSAFE_SUM_RESULT);

    if (e_group_config.core_row == 0 && e_group_config.core_col == 0)
    {
        *safeSum = 0;
        *unsafeSum = 0;
        *mutex = 0;
        *finalBarrier = 0;
        *initSignal = 1;
        startTimer();
    }
    else
    {
        while (*initSignal != 1);
    }

    // Unsafe sum
    for (i = 0; i < 1000; i++)
        sum(unsafeSum);

    // safe sum
    for (i = 0; i < 1000; i++)
    {
        SAFE_OP(sum(safeSum), mutex );
    }

    if (e_group_config.core_row == 0 && e_group_config.core_col == 0)
    {
        while (*finalBarrier != 15);
        stopTimer();
        SAFE_OP(sum(finalBarrier), mutex);
    }
    else
    {
        SAFE_OP(sum(finalBarrier), mutex);
    }

    return 0;
}
