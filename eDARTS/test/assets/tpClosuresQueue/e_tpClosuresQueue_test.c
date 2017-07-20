// Test for the tpClosureQueue

#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"

#include "../tpClosuresQueue/e_tp_definitions.h"

#define INIT_SIGNAL 0x6004
#define FINAL_SIGNAL 0x6008
#define BARRIER_LOCATION 0x600C

// sync Variables
unsigned *initSignal;
unsigned *finalSignal;
darts_barrier_t * myBarrier;
tpClosuresQueue_t * myTpQueue;
unsigned * currentExecTpAddr;
void * myTpHeap;
void * myLocalMemHeap;
void * myDRAMMemHeap;


// Not thread safe
void e_init_runtime() {
    unsigned base0_0Address = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );

    // For sync with the host and finishing the runtime
    initSignal = (unsigned *) (base0_0Address + INIT_SIGNAL);
    finalSignal = (unsigned *) (base0_0Address + FINAL_SIGNAL);

    // pointer to the actual queue
    myTpQueue = (tpClosuresQueue_t *) (base0_0Address + TP_QUEUE_LOCATION);

    // For the codelets to get a graps of their context
    currentExecTpAddr = (unsigned *) (CURRENT_EXECUTING_TP);

    // For memory allocation of ThreadedProcedures and local memory
    myTpHeap = (void *) (TP_HEAP_LOCATION);
    myLocalMemHeap = (void *) (LOCAL_MEM_LOCATION);
    myDRAMMemHeap = (void *)(EXTERNAL_MEM_LOCATION);

    // To create a barrier at the end of each experiment
    myBarrier =(darts_barrier_t *) ( base0_0Address + BARRIER_LOCATION);
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
        darts_barrier_init(myBarrier,16);
        initTpClosuresQueue(myTpQueue,100);
        e_darts_print(" === TP CLOSURE QUEUE TESTING ===\n");

        // Initialization barrier. Have to be done this way
        *initSignal = 1;
    } else {
        while (*initSignal != 1);
    }
}

void e_singleProducerSingleConsumer() {
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
        //Here we have the consumers
        unsigned base0_0Address = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );

        // while the final tp has not been submitted
        // Since we only have one single consumer we can own the queue at the beginning
        unsigned numCreatedTps = 0, i, j, k, stillExecuting = 0;
        ownTpClosureQueue(myTpQueue);
        while (*finalSignal != 1 || stillExecuting == 1) { // Final signal is set by a TP
            stillExecuting = 0;
            if (!isTpClosureQueueEmpty(myTpQueue)) {
                genericTpClosure_t * currentTpClosure = 0;
                // SU scheduling
                // Check if we successfully pop a tpClosure
                if (peakTopElement(myTpQueue,&currentTpClosure) == TPC_QUEUE_SUCCESS_OP) {
                    // TP allocation

                    _tp_metadata_t * actualTP = (_tp_metadata_t *) myTpHeap;
                    *actualTP = currentTpClosure->_metadataCtor(numCreatedTps);
                    numCreatedTps++;
                    // Move the heap pointer
                    myTpHeap = (void *) (((unsigned)myTpHeap) + sizeof(_tp_metadata_t) + (sizeof(syncSlot_t)*actualTP->numSyncSlots));
                    // Memory allocation for Local
                    if (actualTP->sizeLocal != 0) {
                        actualTP->memLocal = myLocalMemHeap;
                        myLocalMemHeap = (void *) (base0_0Address + (unsigned)myLocalMemHeap + actualTP->sizeLocal);
                    }
                    // Memory allocation for DRAM
                    if (actualTP->sizeDRAM != 0) {
                        actualTP->memDRAM = myDRAMMemHeap;
                        myDRAMMemHeap = (void *) ((unsigned)myDRAMMemHeap + actualTP->sizeDRAM);
                    }

                    // Now we can call the user Constructor
                    currentTpClosure->_userInitCtorExec(actualTP,currentTpClosure);

                    // Last thing is to delete that already processed element from the queue
                    popTopElementQueue(myTpQueue);
                }
            }
            // Codelet Scheduling
            // Iterate over the TPs to find codelets ready to execute
            *currentExecTpAddr = (unsigned) (TP_HEAP_LOCATION);
            _tp_metadata_t * currentTp = (_tp_metadata_t *) (*currentExecTpAddr);
            for (i = 0; i < numCreatedTps; i ++) {
                syncSlot_t * currentSyncSlot = (syncSlot_t *) ( (*currentExecTpAddr) + sizeof(_tp_metadata_t));
                // Iterate over the syncSlots to find codelets that are ready to be executed
                for (j = 0; j<currentTp->numSyncSlots; j++) {
                    //e_darts_print("Here %d = %d\n",currentSyncSlot->slotID, currentSyncSlot->currentDep);
                    if (currentSyncSlot->currentDep == 0) {
                        // Ready to execute
                        codelet_t codeletToExec = currentSyncSlot->codeletTemplate;
                        for (k = 0; k < currentSyncSlot->numCodelets; k++) {
                            codeletToExec.fire();
                        }
                        stillExecuting = 1; // TODO This might not work
                        syncSlotResetDep(currentSyncSlot);
                    }
                    // next SyncSlot
                    currentSyncSlot++;
                }
                *currentExecTpAddr = ((*currentExecTpAddr) + sizeof(_tp_metadata_t) + (sizeof(syncSlot_t)*currentTp->numSyncSlots));
                currentTp = (_tp_metadata_t *) (*currentExecTpAddr);
            }
        }
        // We are done with the queue, we disown it
        disownTpClosureQueue(myTpQueue);

        darts_barrier(myBarrier);
    } else if (e_group_config.core_row == 0 && e_group_config.core_col == 1) {
        int i;

        for (i = 0; i < 6 ; i++) {
            simple_tp_tpClosure_t newSimpleTpClosure = _invoke_simple_tp();
            pushTpClosureQueue(myTpQueue,(genericTpClosure_t*)(&newSimpleTpClosure));
        }

        // Create the tpClosure and pushing it
        complex_tp_tpClosure_t newComplexTpClosure = _invoke_complex_tp(5,6);
        pushTpClosureQueue(myTpQueue,(genericTpClosure_t*)(&newComplexTpClosure));

        darts_barrier(myBarrier);
    } else {
        darts_barrier(myBarrier);
    }

}

void e_multipleProducersSingleConsumer() {

}

void e_multipleProducersMultipleConsumers() {

}

void e_checkingPaddingAndErrors() {

}


void e_tpDefiner(darts_barrier_t * threadsBarrier) {
//    volatile my_test_tpClosure_t * tpClosure = (my_test_tpClosure_t *)TP_CLOSURE_LOCATION;
//    *tpClosure = (my_test_tpClosure_t)_invoke_my_test(4,5);
//
//    darts_barrier(threadsBarrier);
//    e_darts_print(" a tpClosure was created \n");
//    // Wait until TP everything is executed
}

void e_tpCreatorExecuter(darts_barrier_t * threadsBarrier) {
//    unsigned base0_0Address = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
//    genericTpClosure_t * tpClosure = (genericTpClosure_t *)(base0_0Address + TP_CLOSURE_LOCATION);
//    _tp_metadata_t * actualTP = (_tp_metadata_t *) TP_LOCATION;
//    // Waiting until TP closure is created by e_tpDefiner via a tpClosure
//    darts_barrier(threadsBarrier);
//
//    // Actually create the TP
//
//    unsigned tpId = 0;
//    // Initialization of metadata;
//    *actualTP = tpClosure->_metadataCtor(tpId);
//
//    // Initialize syncSlots to zero
//    int i;
//    syncSlot_t* theSyncSlot = GET_SYNC_SLOT(*actualTP,0);
//    for ( i = 0; i < actualTP->numSyncSlots; i++)
//    {
//        codelet_t emptyCodelet;
//        initCodelet(&emptyCodelet,0,theSyncSlot,__emptyCodeletFunction);
//        initSyncSlot(theSyncSlot,i,0,0,emptyCodelet,0);
//        theSyncSlot++;
//    }
//    e_darts_print(" Starting runtime %X \n",(unsigned)(*((unsigned*)tpClosure)));
//
//
//    // Allocation of data size of args should be used here by the runtime
//    actualTP->memDRAM = (void *)LOCAL_MEM_LOCATION;
//    actualTP->memLocal = (void *)EXTERNAL_MEM_LOCATION;
//    // Call user initializer
//    tpClosure->_userInitCtorExec(actualTP,tpClosure);
//    // After this the SU scheduler should push the syncSlots to a microscheduler
//
//
//    // Lets log this event
//    e_darts_print(" a TP was created \n");
////
////    // pseudo runtime
//    unsigned firedFlag[4] = {0,0,0,0};
//    for (i = 0; i < actualTP->numSyncSlots; i++) {
//        syncSlot_t * currentSyncSlot = GET_SYNC_SLOT(*actualTP,i);
//        if (currentSyncSlot->currentDep == 0 && firedFlag[i] != 1) {
//            // Here we are supposed to push the codelet to the codeletQueue
//            codelet_t codeletToExec = currentSyncSlot->codeletTemplate;
//            codeletToExec.fire();
//            syncSlotResetDep(currentSyncSlot);
//            firedFlag[i] = 1;
//            i = -1;
//        }
//    }
//    e_darts_print("Runtime finished \n");


}

int main(void)
{
    e_init_runtime();
    e_singleProducerSingleConsumer();

    darts_barrier(myBarrier);
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0)
    {
        e_darts_print("=== END TP TESTING: ===\n");
        *finalSignal = 2;
    }

    return 0;
}
