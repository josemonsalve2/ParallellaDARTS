#include "e_darts_cu_scheduler.h"
#include "e_darts_su.h"


// Scheduler Policies
void cu_scheduler_round_robin() {
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    codeletsQueue_t * thisCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsCUElements.darts_rt_codeletsQueue));
    codelet_t toFire;
    while(darts_rt_alive != 0) {
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
            codeletsQueue_t * thisCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsCUElements.darts_rt_codeletsQueue));
            toDecDep->codeletTemplate.codeletID = i;
            while (pushCodeletQueue(thisCodeletQueue, &(toDecDep->codeletTemplate)) != CODELET_QUEUE_SUCCESS_OP);
        }
    }
}

// addCodelet Policies
void addCodeletSelfQueue(codelet_t * toAdd) {
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    codeletsQueue_t * thisCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsCUElements.darts_rt_codeletsQueue));
    while (pushCodeletQueue(thisCodeletQueue, toAdd) != CODELET_QUEUE_SUCCESS_OP);
}

// invoke Policies
void invokeSelfQueue(genericTpClosure_t * tpClosure) {
    unsigned suCodeID = ((0xFFF00000) & ((unsigned)_dartsCUElements.mySUElements));
    tpClosuresQueue_t * suTPCQueue = (tpClosuresQueue_t *) DARTS_APPEND_COREID(suCodeID,&(_dartsCUElements.mySUElements->darts_rt_tpclosuresQueue));
    while(pushTpClosureQueue(suTPCQueue, tpClosure) != TPC_QUEUE_SUCCESS_OP);
}

void darts_set_cu_scheduler(scheduler_t* cu_scheduler, cu_scheduler_selector cu_scheduler_policy) {
    switch(cu_scheduler_policy) {
    case CU_ROUND_ROBIN:
        cu_scheduler->policy = cu_scheduler_round_robin;
        cu_scheduler->decDep = decDepAndPush;
        cu_scheduler->invokeTP = invokeSelfQueue;
        cu_scheduler->addCodelet = addCodeletSelfQueue;
        break;
    default:
        break;
    }
}
