#include <stdio.h>
#include <stdlib.h>
#include "e_DARTS.h"
#include "e-lib.h"
#include "e_tp_definitions.h"

//scheduler_t dartsRtScheduler;

void func1 (codelet_t * t) {
    e_darts_print("Hello I am func1");
}

void func2 (genericTpClosure_t * t) {
    e_darts_print("Hello I am func1");
}

void func3 (syncSlot_t * t) {
    e_darts_print("Hello I am func1");
}

int main(void)
{
    dartsRtScheduler.addCodelet = func1;
    dartsRtScheduler.decDep = func3;
    dartsRtScheduler.invokeTP = func2;
    sizeOfSUElements.codeletsQueuePadding[0] = 0;
    sizeOfCUElements.codeletsQueuePadding[0] = 0;
    syncSlot_t useless;
    DEC_DEP(&useless);
    INVOKE(simple_tp);

    return 0;
}

