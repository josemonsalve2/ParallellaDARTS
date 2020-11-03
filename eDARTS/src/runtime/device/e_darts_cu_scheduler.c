#include "e_darts_cu_scheduler.h"
#include "e_darts_su.h"
#include "e_darts_print.h"


// Scheduler Policies
void cu_scheduler_round_robin() {
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    cu_runtime_elements_t * myCUElements = (cu_runtime_elements_t *) DARTS_APPEND_COREID(thisCoreID, &(_dartsCUElements));
    codeletsQueue_t * thisCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsCUElements.darts_rt_codeletsQueue));
    codelet_t toFire;
    while(darts_rt_alive != 0) {
        if (popCodeletQueue(thisCodeletQueue, &toFire) == CODELET_QUEUE_SUCCESS_OP ) {
            e_darts_print("codelet popped in CU codelet queue\n");
            myCUElements->currentThreadedProcedure = toFire.syncSlot->tpFrame; //is this too much dereferencing?
            toFire.fire();
            e_darts_print("codelet completed firing in CU\n");
        }
    }
}

// decDep Policies
void cu_decDepAndPush(syncSlot_t * toDecDep){
    int i;
    e_darts_print("cu_decDep called\n");
    // Check if dependencies are zero
    if (syncSlotDecDep(toDecDep) == 0) {
        e_darts_print("cu_decDep called - 0 deps\n");
	if (toDecDep->codeletTemplate.codeletID == 0xFFFFFFFF) { //if final codelet, push to SU codelet queue instead
            //codeletsQueue_t * suCodeletQueue = (codeletsQueue_t *) &(_dartsCUElements.mySUElements->darts_rt_codeletsQueue);
	    // SU hardcoded below
	    codeletsQueue_t * suCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(0x808,&(_dartsCUElements.mySUElements->darts_rt_codeletsQueue));
	    while (pushCodeletQueue(suCodeletQueue, &(toDecDep->codeletTemplate)) != CODELET_QUEUE_SUCCESS_OP);
        }
        // Push as many codelets as possible
        else { //normal codelet
            for (i = 0; i < toDecDep->numCodelets; i++) {
                unsigned thisCoreID;
                DARTS_GETCOREID(thisCoreID);
                codeletsQueue_t * thisCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsCUElements.darts_rt_codeletsQueue));
                toDecDep->codeletTemplate.codeletID = i;
                while (pushCodeletQueue(thisCodeletQueue, &(toDecDep->codeletTemplate)) != CODELET_QUEUE_SUCCESS_OP);
            }
        }
    }
    e_darts_print("cu_decDep returning\n");
}

// addCodelet Policies
void cu_addCodeletSelfQueue(codelet_t * toAdd) {
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    codeletsQueue_t * thisCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsCUElements.darts_rt_codeletsQueue));
    while (pushCodeletQueue(thisCodeletQueue, toAdd) != CODELET_QUEUE_SUCCESS_OP);
}

// invoke Policies
void cu_invokeSelfQueue(genericTpClosure_t * tpClosure) {
    unsigned suCodeID = ((0xFFF00000) & ((unsigned)_dartsCUElements.mySUElements));
    tpClosuresQueue_t * suTPCQueue = (tpClosuresQueue_t *) DARTS_APPEND_COREID(suCodeID,&(_dartsCUElements.mySUElements->darts_rt_tpclosuresQueue));
    while(pushTpClosureQueue(suTPCQueue, tpClosure) != TPC_QUEUE_SUCCESS_OP);
}

void darts_set_cu_scheduler(scheduler_t* cu_scheduler, cu_scheduler_selector cu_scheduler_policy) {
    switch(cu_scheduler_policy) {
    case CU_ROUND_ROBIN:
        cu_scheduler->policy = cu_scheduler_round_robin;
        cu_scheduler->decDep = cu_decDepAndPush;
        cu_scheduler->invokeTP = cu_invokeSelfQueue;
        cu_scheduler->addCodelet = cu_addCodeletSelfQueue;
        break;
    default:
        break;
    }
}
