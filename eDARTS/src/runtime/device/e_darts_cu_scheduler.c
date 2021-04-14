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
            //e_darts_print("codelet popped in CU codelet queue\n");
	    //SU hardcoded here
            myCUElements->currentThreadedProcedure = (_tp_metadata_t *) DARTS_APPEND_COREID(0x808,toFire.syncSlot->tpFrame); //is this too much dereferencing?
	    myCUElements->currentCodelet = (codelet_t *) DARTS_APPEND_COREID(thisCoreID,&toFire); // set currentCodelet to toFire
	    //e_darts_print("codelet's syncSlot located at %x\n", (unsigned)toFire.syncSlot);
	    //e_darts_print("codelet's syncSlot tpFrame -> %x\n", toFire.syncSlot->tpFrame);
	    //e_darts_print("codelet's syncSlot tpFrame pointer address -> %x\n", &(toFire.syncSlot->tpFrame));
	    //e_darts_print("CUElements current TP at %x\n", (unsigned)myCUElements->currentThreadedProcedure);
            toFire.fire();
            //e_darts_print("codelet completed firing in CU\n");
        }
    }
}

// decDep Policies
void cu_decDepAndPush(syncSlot_t * toDecDep){
    int i;
    // Check if dependencies are zero
    if (syncSlotDecDep(toDecDep) == 0) {
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
                //codeletsQueue_t * thisCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsCUElements.darts_rt_codeletsQueue));
		//SU hardcoded below
	        codeletsQueue_t * suCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(0x808,&(_dartsCUElements.mySUElements->darts_rt_codeletsQueue));
                toDecDep->codeletTemplate.codeletID = i;
                //while (pushCodeletQueue(thisCodeletQueue, &(toDecDep->codeletTemplate)) != CODELET_QUEUE_SUCCESS_OP);
		//int result = pushCodeletQueue(thisCodeletQueue, &(toDecDep->codeletTemplate));
		int result = pushCodeletQueue(suCodeletQueue, &(toDecDep->codeletTemplate));
                while(result != CODELET_QUEUE_SUCCESS_OP) {
                    if (result == CODELET_QUEUE_NOT_ENOUGH_SPACE) {
                        e_darts_print("SU codelet queue full\n");
                        //wait for a sec
                        int j = 0;
                        while(j<1000000*i) j++;
                    } //if result
                    result = pushCodeletQueue(suCodeletQueue, &(toDecDep->codeletTemplate));
                } //while
            } //for codelet ID
        } //else (normal codelet)
    } //if decDep 0
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
    e_darts_print("inside cu invoke\n");
    //unsigned suCodeID = ((0xFFF00000) & ((unsigned)_dartsCUElements.mySUElements));
    //tpClosuresQueue_t * suTPCQueue = (tpClosuresQueue_t *) DARTS_APPEND_COREID(suCodeID,&(_dartsCUElements.mySUElements->darts_rt_tpclosuresQueue));
    // SU hardcoded here
    tpClosuresQueue_t *suTPCQueue = (tpClosuresQueue_t *) DARTS_APPEND_COREID(0x808, &(_dartsCUElements.mySUElements->darts_rt_tpclosuresQueue));
    //while(pushTpClosureQueue(suTPCQueue, tpClosure) != TPC_QUEUE_SUCCESS_OP);
    int result = pushTpClosureQueue(suTPCQueue, tpClosure);
    while(result != TPC_QUEUE_SUCCESS_OP) {
        result = pushTpClosureQueue(suTPCQueue, tpClosure);
	e_darts_print("CU push to TPClosure queue returned %d\n", result);
    }
    e_darts_print("cu finished invoke\n");
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
