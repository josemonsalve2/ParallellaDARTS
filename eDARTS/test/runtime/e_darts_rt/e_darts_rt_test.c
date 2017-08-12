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
    e_darts_print("Hello I am func2");
}

void func3 (syncSlot_t * t) {
    e_darts_print("Hello I am func3");
}

int main(void)
{
    dartsRtScheduler.addCodelet = func1;
    dartsRtScheduler.decDep = func3;
    dartsRtScheduler.invokeTP = func2;
    _dartsSUElements.codeletsQueuePadding[0] = 0;
    _dartsCUElements.codeletsQueuePadding[0] = 0;
    e_darts_print("\n sizeOfdartRTScheduler = %x\n",sizeof(dartsRtScheduler));
    e_darts_print("\n addCU = %x\n",&_dartsCUElements);
    syncSlot_t useless;
    DEC_DEP(&useless);
    INVOKE(simple_tp);

    return 0;
}

