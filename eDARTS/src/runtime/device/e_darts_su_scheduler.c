#include "e_darts_su_scheduler.h"

// Scheduler Policies
void su_scheduler_round_robin() {
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    tpClosuresQueue_t * thisTPCQueue = (tpClosuresQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements.darts_rt_tpclosuresQueue));
    codeletsQueue_t * thisCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements.darts_rt_codeletsQueue));
    codelet_t toFire;
    genericTpClosure_t * tpClosureToBuild;

    while(darts_rt_alive != 0) {
        // Do the ThreadedProcedures
        if(ownTpClosureQueue(thisTPCQueue) == TPC_QUEUE_SUCCESS_OP) {
            if (peakTopElement(thisTPCQueue,&tpClosureToBuild) == TPC_QUEUE_SUCCESS_OP) {
//                _tp_metadata_t * actualTP = (_tp_metadata_t *) ;
//                *actualTP = currentTpClosure->_metadataCtor(numCreatedTps);
//                numCreatedTps++;
//                // Move the heap pointer
//                myTpHeap = (void *) (((unsigned)myTpHeap) + sizeof(_tp_metadata_t) + (sizeof(syncSlot_t)*actualTP->numSyncSlots));
//                // Memory allocation for Local
//                if (actualTP->sizeLocal != 0) {
//                    actualTP->memLocal = myLocalMemHeap;
//                    myLocalMemHeap = (void *) (base0_0Address + (unsigned)myLocalMemHeap + actualTP->sizeLocal);
//                }
//                // Memory allocation for DRAM
//                if (actualTP->sizeDRAM != 0) {
//                    actualTP->memDRAM = myDRAMMemHeap;
//                    myDRAMMemHeap = (void *) ((unsigned)myDRAMMemHeap + actualTP->sizeDRAM);
//                }
//
//                // Now we can call the user Constructor
//                currentTpClosure->_userInitCtorExec(actualTP,currentTpClosure);
//
//                // Last thing is to delete that already processed element from the queue
//                popTopElementQueue(myTpQueue);
            }

            disownTpClosureQueue(thisTPCQueue);
        }

        // Try to execute codelets // TODO don't execute, distribute, otherwise, execute
        if (popCodeletQueue(thisCodeletQueue, &toFire) == CODELET_QUEUE_SUCCESS_OP ) {
            toFire.fire();
        }
    }
}

// decDep Policies
void decDepAndPush(syncSlot_t * toDecDep){
    int i;
    // Check if dependencies are zero
    if (syncSlotDecDep(toDecDep) == 0) {
        // Push as many codelets as possible
        for (i = 0; i < toDecDep->numCodelets; i++) {
            unsigned thisCoreID;
            DARTS_GETCOREID(thisCoreID);
            codeletsQueue_t * thisCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements.darts_rt_codeletsQueue));
            toDecDep->codeletTemplate.codeletID = i;
            while(pushCodeletQueue(thisCodeletQueue, &(toDecDep->codeletTemplate)) != CODELET_QUEUE_SUCCESS_OP);
        }
    }
}

// addCodelet Policies
void addCodeletSelfQueue(codelet_t * toAdd) {
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    codeletsQueue_t * thisCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements.darts_rt_codeletsQueue));
    while(pushCodeletQueue(thisCodeletQueue, toAdd) != CODELET_QUEUE_SUCCESS_OP);
}

// invoke Policies
void invokeSelfQueue(genericTpClosure_t * tpClosure) {
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    tpClosuresQueue_t * thisTPCQueue = (tpClosuresQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements.darts_rt_tpclosuresQueue));
    while(pushTpClosureQueue(thisTPCQueue, tpClosure) != TPC_QUEUE_SUCCESS_OP);
}

void darts_set_su_scheduler(scheduler_t* su_scheduler, su_scheduler_selector su_scheduler_policy) {
    switch(su_scheduler_policy) {
    case SU_ROUND_ROBIN:
        su_scheduler->policy = su_scheduler_round_robin;
        su_scheduler->decDep = decDepAndPush;
        su_scheduler->invokeTP = invokeSelfQueue;
        su_scheduler->addCodelet = addCodeletSelfQueue;
        break;
    default:
        break;
    }
}
