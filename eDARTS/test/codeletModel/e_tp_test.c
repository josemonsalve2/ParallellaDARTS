//First implementation of a codeletsQueue for the parallella

#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "e_tp_definition_test.h"


#define INIT_SIGNAL 0x6004
#define FINAL_SIGNAL 0x6008
#define BARRIER_LOCATION 0x600C



typedef void (*codeletFunction)();

//function to execute
//void sum() __attribute__((section (".internaltext")));

void e_tpDefiner(darts_barrier_t * threadsBarrier) {
    my_test_tpClosure_t * tpClosure = (my_test_tpClosure_t *)TP_CLOSURE_LOCATION;
    *tpClosure = _invoke_my_test(4,5);
    e_darts_print(" a tpClosure was created \n");
    darts_barrier(threadsBarrier);
    // Wait until TP everything is executed
}

void e_tpCreatorExecuter(darts_barrier_t * threadsBarrier) {
    unsigned base0_0Address = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
    genericTpClosure_t * tpClosure = (genericTpClosure_t *)(base0_0Address + TP_CLOSURE_LOCATION);
    _tp_metadata_t * actualTP = (_tp_metadata_t *) TP_LOCATION;
    // Waiting until TP closure is created by e_tpDefiner via a tpClosure
    darts_barrier(threadsBarrier);

    // Actually create the TP
    unsigned tpId = 0;

    // Initialization of metadata;
    *actualTP = tpClosure->_metadataCtor(tpId);
    e_darts_print(" Starting runtime %X \n",(unsigned)(*((unsigned *)(tpClosure->_metadataCtor)+2)));


    // Allocation of data size of args should be used here by the runtime
    actualTP->memDRAM = (void *)LOCAL_MEM_LOCATION;
    actualTP->memLocal = (void *)EXTERNAL_MEM_LOCATION;
    // Call user initializer
    tpClosure->_userInitCtorExec(actualTP,tpClosure);
    // After this the SU scheduler should push the syncSlots to a microscheduler


    // Lets log this event
    e_darts_print(" a TP was created \n");

    // pseudo runtime
    int i, somethingToExecute;
    for (i = 0; i < actualTP->numSyncSlots; i++)
    {
        somethingToExecute = 0;
        syncSlot_t * currentSyncSlot = GET_SYNC_SLOT(*actualTP,i);
        if (currentSyncSlot->currentDep == 0) {
            // Here we are supposed to push the codelet to the codeletQueue
            codelet_t codeletToExec = currentSyncSlot->codeletTemplate;
            codeletToExec.fire();
        }
    }

}

int main(void)
{

    unsigned base0_0Address = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
    unsigned *initSignal = (unsigned *) (base0_0Address + INIT_SIGNAL);
    unsigned *finalSignal = (unsigned *) (base0_0Address + FINAL_SIGNAL);
    darts_barrier_t * myBarrier =(darts_barrier_t *) ( base0_0Address + BARRIER_LOCATION);

    // core (0,0) is producer
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0)
    {
        darts_barrier_init(myBarrier,16);
        *initSignal = 1;
        e_darts_print(" === TP TESTING ===\n");
        e_tpDefiner(myBarrier);
    }
    else if (e_group_config.core_row == 0 && e_group_config.core_col == 1)// core (0,1) is consumer
    {
        while (*initSignal == 0);
        e_tpCreatorExecuter(myBarrier);
    } else {
        darts_barrier(myBarrier);
    }

    darts_barrier(myBarrier);
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0)
    {
        e_darts_print("\n === END TP TESTING: ===\n");
        *finalSignal = 1;
    }


    return 0;
}
