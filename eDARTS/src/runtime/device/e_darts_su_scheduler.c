#include "e_darts_su_scheduler.h"
#include "e_darts_print.h"
#include "e_darts_mailbox.h"

//tp_heap_space_t _dartsTpHeap __attribute__ ((section(".dartsTpHeap")));
extern tp_heap_space_t _dartsTpHeap;
extern tp_heap_space_t _tpDataHeap;
//extern unsigned base0_0Address;

#define LOCAL_MEM_LOCATION 0x7000
#define EXTERNAL_MEM_LOCATION 0x8f000000

// Scheduler Policies
void su_scheduler_round_robin() {
    unsigned base0_0Address = (unsigned) e_get_global_address(0, 0, 0x0000);
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    tpClosuresQueue_t * thisTPCQueue = (tpClosuresQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements.darts_rt_tpclosuresQueue));
    codeletsQueue_t * thisCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements.darts_rt_codeletsQueue));
    void *myTpHeap = (void *) &(_dartsTpHeap);
    codelet_t toFire;
    genericTpClosure_t * tpClosureToBuild;
    unsigned cuIndex = 0;
    unsigned numCreatedTps = 0;
    while(darts_rt_alive != 0) {
        // Do the ThreadedProcedures
        // with this program flow, the SU will always go to initialize TPs from the queue if possible before
	// distributing codelets, so if the queue is stacked there's a possibility of issues i.e. if the SUs 
	// codelet queue fills up before the TP closure queue is empty, there will be a huge issue since
	// codelets have to go into the SU codelet queue before they're distributed. Maybe this should change	
        if(ownTpClosureQueue(thisTPCQueue) == TPC_QUEUE_SUCCESS_OP) {
            if (peakTopElement(thisTPCQueue,&tpClosureToBuild) == TPC_QUEUE_SUCCESS_OP) {
                _tp_metadata_t * actualTP = myTpHeap;
                *actualTP = tpClosureToBuild->_metadataCtor(numCreatedTps);
                numCreatedTps++;
                // Move the heap pointer
		// all the heap variables need somewhere to go. Where is a large continuous memory block?
		// make e_mallocs for each spot (the TP, local, and DRAM)?
		e_darts_print("TP metadata located at %x\n", (unsigned)myTpHeap);
                myTpHeap = (void *) (((unsigned)myTpHeap) + sizeof(_tp_metadata_t) + (sizeof(syncSlot_t)*actualTP->numSyncSlots));
                // Memory allocation for Local
                if (actualTP->sizeLocal != 0) {
                    //heap is anchored here for now
                    void *myLocalMemHeap = (void *) &(_tpDataHeap);
                    actualTP->memLocal = myLocalMemHeap;
                    myLocalMemHeap = (void *) (base0_0Address + (unsigned)myLocalMemHeap + actualTP->sizeLocal);
                }
                // Memory allocation for DRAM
                if (actualTP->sizeDRAM != 0) {
                    void *myDRAMMemHeap = (void *) EXTERNAL_MEM_LOCATION;
		    //need to change this to tpHeap structure
                    actualTP->memDRAM = myDRAMMemHeap;
                    myDRAMMemHeap = (void *) ((unsigned)myDRAMMemHeap + actualTP->sizeDRAM);
                }

                // Now we can call the user Constructor
                tpClosureToBuild->_userInitCtorExec(actualTP,tpClosureToBuild);


                // Last thing is to delete that already processed element from the queue
                popTopElementQueue(thisTPCQueue);
	        e_darts_print("TP initialization done\n");
            }

            disownTpClosureQueue(thisTPCQueue);
        }

        // Try to execute codelets // TODO don't execute, distribute, otherwise, execute
	// unique case: when the codelet is the termination codelet, distribute a copy to all cores
	// in the future, the SU will need access to the CAM so that it knows what cores to distribute to,
	// or reorganize the CUElements array in SUElements
	// Seems like all codelets get pushed to the codelet queue of whichever core executes the decDep,
	// but SU should be only doing TPs if possible, so push from SU codelet queue to a CU codelet queue
        if (popCodeletQueue(thisCodeletQueue, &toFire) == CODELET_QUEUE_SUCCESS_OP ) {
            codeletsQueue_t *cuCodeletQueue = (codeletsQueue_t *) &(_dartsSUElements.myCUElements[cuIndex+1]->darts_rt_codeletsQueue);
	    int i;
	    if (toFire.codeletID == 0xFFFFFFFF) { //if final codelet
                for(i=1; i<16; i++) { // push to all CU's codelet queues
                    cuCodeletQueue = (codeletsQueue_t *) &(_dartsSUElements.myCUElements[i]->darts_rt_codeletsQueue);
		    unsigned response = 5;
		    while(response != CODELET_QUEUE_SUCCESS_OP) {
                        response = pushCodeletQueue(cuCodeletQueue, &toFire);
			e_darts_print("pushed final codelet to CU %d with response %d\n", i, response);
                    }
		    while(pushCodeletQueue(cuCodeletQueue, &toFire) != CODELET_QUEUE_SUCCESS_OP);
                }
                toFire.fire(); //then fire
            }
	    else {
	        // for loop tries to push to the queue of each CU. starts at cuIndex so that it won't keep trying to push
                // to the first CU until its full. This way if all pushes are sucessful it will push to CU 0, then 1, then 2,
                // etc. but if it fails it tries to push to the next one. Basically, distributes evenly where possible,
                // always distributes if possible, SU fires the codelet itself otherwise
		//e_darts_print("Trying to push codelet to queue %d\n", cuIndex+1);
		//unsigned response = 4;
                //while(response != CODELET_QUEUE_SUCCESS_OP) {
                //    cuCodeletQueue = (codeletsQueue_t *) &(_dartsSUElements.myCUElements[cuIndex%15+1]->darts_rt_codeletsQueue);
                //    response = pushCodeletQueue(cuCodeletQueue, &toFire);
		//    e_darts_print("tried to push to CU %d with response %d\n", cuIndex%15+1, response);
		//    cuIndex = (cuIndex + 1) % 15;;
                //}
	        for(i=1; pushCodeletQueue(cuCodeletQueue, &toFire) != CODELET_QUEUE_SUCCESS_OP && i<15; i++) {
		    e_darts_print("Failed to push codelet to queue %d\n", (cuIndex+i-1)%15+1);
                    cuCodeletQueue = (codeletsQueue_t *) &(_dartsSUElements.myCUElements[(cuIndex+i)%15 + 1]->darts_rt_codeletsQueue);
                }
		e_darts_print("Codelet pushed to queue %d\n", (cuIndex+i)%15+1);
	        if(i == 15) { //if for loop failed to push to one of the CU queues
		    e_darts_print("SU firing codelet\n");
                    toFire.fire();
                }
            } //else
            cuIndex = (cuIndex + 1) % 15; //index for which codelet queue to push to. stays in [0, 14]. 
	                                  // 1 is added before access so will be [1, 15] (hardcoded for SU at 0)
        } //if codelet popped success
	message signal;
	if (e_darts_receive_signal(&signal) == NM_REQUEST_SU_PROVIDE) {
            e_darts_print("NM_REQUEST_SU_PROVIDE received at SU\n");
	    signal = SU_MAILBOX_ACCEPT;
	    e_darts_send_signal(&signal);
	}
    } //while
}

// decDep Policies
void su_decDepAndPush(syncSlot_t * toDecDep){
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
	    //int x = pushCodeletQueue(thisCodeletQueue, &(toDecDep->codeletTemplate));
	    //while(x != CODELET_QUEUE_SUCCESS_OP) {
                //x = pushCodeletQueue(thisCodeletQueue, &(toDecDep->codeletTemplate));
            //}
        }
    }
}

// addCodelet Policies
void su_addCodeletSelfQueue(codelet_t * toAdd) {
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    codeletsQueue_t * thisCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements.darts_rt_codeletsQueue));
    while(pushCodeletQueue(thisCodeletQueue, toAdd) != CODELET_QUEUE_SUCCESS_OP);
}

// invoke Policies
void su_invokeSelfQueue(genericTpClosure_t * tpClosure) {
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    tpClosuresQueue_t * thisTPCQueue = (tpClosuresQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements.darts_rt_tpclosuresQueue));
    while(pushTpClosureQueue(thisTPCQueue, tpClosure) != TPC_QUEUE_SUCCESS_OP);
}

void darts_set_su_scheduler(scheduler_t* su_scheduler, su_scheduler_selector su_scheduler_policy) {
    switch(su_scheduler_policy) {
    case SU_ROUND_ROBIN:
        su_scheduler->policy = su_scheduler_round_robin;
        su_scheduler->decDep = su_decDepAndPush;
        su_scheduler->invokeTP = su_invokeSelfQueue;
        su_scheduler->addCodelet = su_addCodeletSelfQueue;
        break;
    default:
        break;
    }
}
