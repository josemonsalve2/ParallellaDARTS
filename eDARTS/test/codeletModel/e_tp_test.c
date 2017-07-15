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
    volatile my_test_tpClosure_t * tpClosure = (my_test_tpClosure_t *)TP_CLOSURE_LOCATION;
    *tpClosure = (my_test_tpClosure_t)_invoke_my_test(4,5);

    darts_barrier(threadsBarrier);
    e_darts_print(" a tpClosure was created \n");
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

    // Initialize syncSlots to zero
    int i;
    syncSlot_t* theSyncSlot = GET_SYNC_SLOT(*actualTP,0);
    for ( i = 0; i < actualTP->numSyncSlots; i++)
    {
        codelet_t emptyCodelet;
        initCodelet(&emptyCodelet,0,theSyncSlot,__emptyCodeletFunction);
        initSyncSlot(theSyncSlot,i,0,0,emptyCodelet,0);
        theSyncSlot++;
    }
    e_darts_print(" Starting runtime %X \n",(unsigned)(*((unsigned*)tpClosure)));


    // Allocation of data size of args should be used here by the runtime
    actualTP->memDRAM = (void *)LOCAL_MEM_LOCATION;
    actualTP->memLocal = (void *)EXTERNAL_MEM_LOCATION;
    // Call user initializer
    tpClosure->_userInitCtorExec(actualTP,tpClosure);
    // After this the SU scheduler should push the syncSlots to a microscheduler


    // Lets log this event
    e_darts_print(" a TP was created \n");
//
//    // pseudo runtime
    unsigned firedFlag[4] = {0,0,0,0};
    for (i = 0; i < actualTP->numSyncSlots; i++) {
        syncSlot_t * currentSyncSlot = GET_SYNC_SLOT(*actualTP,i);
        if (currentSyncSlot->currentDep == 0 && firedFlag[i] != 1) {
            // Here we are supposed to push the codelet to the codeletQueue
            codelet_t codeletToExec = currentSyncSlot->codeletTemplate;
            codeletToExec.fire();
            syncSlotResetDep(currentSyncSlot);
            firedFlag[i] = 1;
            i = -1;
        }
    }
    e_darts_print("Runtime finished \n");


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
        e_darts_print(" === TP TESTING === %x\n",BARRIER_LOCATION + sizeof(darts_barrier_t));
        e_tpDefiner(myBarrier);
    }
    else if (e_group_config.core_row == 0 && e_group_config.core_col == 1)// core (0,1) is consumer
    {
        while (*initSignal == 0);
        e_tpCreatorExecuter(myBarrier);

    } else {
        while (*initSignal == 0);
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
