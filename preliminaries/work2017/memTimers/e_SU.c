#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "common.h"
#include "codeletsQueue.h"


void sum()
{
    unsigned *value;
    int i,j;
    value = (unsigned *) 0x2228;
    unsigned initialTimerValue = 0xffffffff;
    unsigned configReg = 0, countResult;
    //Stop the timer
    for (j = 0; j < 100; j++)
    {
        initialTimerValue = 0xffffffff;
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
        for (i = 0; i < COUNT_TO; ++i);

        // Stop the timer and get value
        configReg = configReg & 0xffffff0f;// Set timer to count clock
        asm("movfs %[configValue],config\n\t"
            "movfs %[timerValue],ctimer0"
            : [configValue] "=r" (configReg), [timerValue] "=r" (countResult)
            : // No inputs
            );
        value[j] = (0xffffffff - countResult);
    }    
}

void end_rt()
{
    unsigned *cuDone,*suDone;
    cuDone = (unsigned *) CU_DONE_ADDR;
    *cuDone = 1;
    suDone = (unsigned *) (*((unsigned *)CU_SU_BASE_ADDR) + SU_CU_DONE+ 4*MY_CU_NUM);
    *suDone = 1;
    
}

typedef struct PACKED SU_states_s
{
    unsigned startSignal;
    unsigned done;
    unsigned suBaseAddress;
    unsigned signal[NUM_CU];
    unsigned cuBaseAddress[NUM_CU];
    unsigned cuDone[NUM_CU];
} SU_states_t;

void init_SU_states(SU_states_t ** states)
{
    int i;
    *states = (SU_states_t *) SU_STATES_BASE;
    (*states)->done = 0;
    (*states)->suBaseAddress = (unsigned) e_get_global_address( SU_ROW , SU_COL , 0x0000 );
    for ( i = 0; i < NUM_CU; ++i )
    {
        (*states)->cuBaseAddress[i] = (unsigned) e_get_global_address( (i+1) / 4 , (i+1) % 4  , 0x0000 );
        (*states)->cuDone[i] = 0;
    }
}

int main(void)
{
    SU_states_t *states;
    
    init_SU_states(&states);
    
    int i,j;
   
    //for global memory of the CUs
    codeletsQueue_t * codeletQueue;
    unsigned cuCodeletQueueAddr;

    sum();
    
    //Wait for host
    while(states->startSignal != 0);

    //Wait for CUs to be ready
    for (i = 0; i < NUM_CU; ++i)
    {
        while (states->signal[i] != 0);
    }
    
    for (i = 0; i < NUM_CU; ++i)
    {
        cuCodeletQueueAddr = ((unsigned)(states->cuBaseAddress[i] + CU_CODQUEUE_ADDR));
        codeletQueue = (codeletsQueue_t *) (cuCodeletQueueAddr);
        for ( j = 0; j < 1; ++j )
        {
            while ( pushCodeletQueue(codeletQueue, (unsigned) ( states->suBaseAddress + ((unsigned) &sum))) == 1);
        }
        //Send final codelet
        while ( pushCodeletQueue(codeletQueue, (unsigned) ( states->suBaseAddress + ((unsigned) &end_rt))) == 1);
        
        //We want to execute one by one with as little NOC interfeerance as possible
        while ( states->cuDone[i] == 0);

    }
    
//     for (i = 0; i < NUM_CU; i++)
//     {
// 
//     }
    
    states->done = 1;
    return 0;
}

