#include "e_darts_su_scheduler.h"
#include "e_darts_print.h"
#include "e_darts_mailbox.h"

extern tp_heap_space_t _dartsTpHeap;
extern tp_data_heap_space_t _tpDataHeap;
extern tp_data_heap_DRAM_space_t _tpDataHeapDRAM;
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
    void *myLocalMemHeap = (void *) &(_tpDataHeap);
    //void *myDRAMMemHeap = (void *) EXTERNAL_MEM_LOCATION;
    void *myDRAMMemHeap = (void *) &(_tpDataHeapDRAM);
    mailbox_t suMailbox;
    mailbox_t nmMailbox;
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
                    actualTP->memLocal = myLocalMemHeap;
                    myLocalMemHeap = (void *) (base0_0Address + (unsigned)myLocalMemHeap + actualTP->sizeLocal);
                }
                // Memory allocation for DRAM
                if (actualTP->sizeDRAM != 0) {
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
	//int pop_result = popCodeletQueue(thisCodeletQueue, &toFire);
	//e_darts_print("SU's codelet pop returns %d\n", pop_result);
	//if (pop_result == CODELET_QUEUE_SUCCESS_OP) {
        if (popCodeletQueue(thisCodeletQueue, &toFire) == CODELET_QUEUE_SUCCESS_OP ) {
	    e_darts_print("SU popped codelet to deploy\n");
            codeletsQueue_t *cuCodeletQueue = (codeletsQueue_t *) &(_dartsSUElements.myCUElements[cuIndex+1]->darts_rt_codeletsQueue);
	    int i;
	    if (toFire.codeletID == 0xFFFFFFFF) { //if final codelet
                pushFinalCodelets(cuCodeletQueue, &toFire);
            }
	    else {
		deployCodelet(cuCodeletQueue, cuIndex, &toFire);
            } //else
            cuIndex = (cuIndex + 1) % 15; //index for which codelet queue to push to. stays in [0, 14]. 
	                                  // 1 is added before access so will be [1, 15] (hardcoded for SU at 0)
        } //if codelet popped success
	suMailboxCheck(&suMailbox, &nmMailbox);
    } //while
}

inline void pushFinalCodelets(codeletsQueue_t *cuCodeletQueue, codelet_t *toFire)
{
    for(int i=1; i<16; i++) { // push to all CU's codelet queues
        cuCodeletQueue = (codeletsQueue_t *) &(_dartsSUElements.myCUElements[i]->darts_rt_codeletsQueue);
        unsigned response = 5;
        while(response != CODELET_QUEUE_SUCCESS_OP) {
            response = pushCodeletQueue(cuCodeletQueue, toFire);
            e_darts_print("pushed final codelet to CU %d with response %d\n", i, response);
        }
        while(pushCodeletQueue(cuCodeletQueue, toFire) != CODELET_QUEUE_SUCCESS_OP);
    }
    toFire->fire(); //then fire
}

inline void deployCodelet(codeletsQueue_t *cuCodeletQueue, unsigned cuIndex, codelet_t *toFire)
{
    int i;
    for(i=1; pushCodeletQueue(cuCodeletQueue, toFire) != CODELET_QUEUE_SUCCESS_OP && i<15; i++) {
        e_darts_print("Failed to push codelet to queue %d\n", (cuIndex+i-1)%15+1);
        cuCodeletQueue = (codeletsQueue_t *) &(_dartsSUElements.myCUElements[(cuIndex+i)%15 + 1]->darts_rt_codeletsQueue);
    }
    if(i == 15) { //if for loop failed to push to one of the CU queues
        e_darts_print("SU firing codelet\n");
        toFire->fire();
    }
    else {
    	e_darts_print("Codelet %d  pushed to queue %d\n", toFire->codeletID, (cuIndex+i)%15+1);
    }
}	

inline void suMailboxCheck(mailbox_t *suMailbox, mailbox_t *nmMailbox)
{
    if (!e_darts_get_ack()) { //if message isn't acked i.e. SU hasn't seen it yet
        e_darts_receive_data(suMailbox); //intrinsically acks the message and data
        //respond to the message here: for now a print
        e_darts_print("SU received message %d\n", suMailbox->signal);
	e_darts_print("SU received unsigned %u and int %d\n", e_darts_data_convert_to_unsigned(suMailbox->data), e_darts_data_convert_to_int(&(suMailbox->data[4])));
        //nmMailbox->msg_header.msg_type = DATA;
        //nmMailbox->msg_header.size = (unsigned) sizeof(unsigned);
        //nmMailbox->signal = SU_MAILBOX_ACCEPT;
	e_darts_fill_mailbox(nmMailbox, DATA, sizeof(int) + sizeof(unsigned), SU_MAILBOX_ACCEPT);
        //don't have to make msg pointer valid for now because we don't use it in the api
	e_darts_int_convert_to_data(-1, nmMailbox->data);
        e_darts_unsigned_convert_to_data(11U, &(nmMailbox->data[4])); //converts unsigned 3 to 4 char bytes and places in data array
        e_darts_print("SU sending -1 and 11U\n");
	e_darts_print("SU sending raw data %x%x%x%x %x %x %x %x\n", nmMailbox->data[0], nmMailbox->data[1], nmMailbox->data[2], nmMailbox->data[3], nmMailbox->data[4], nmMailbox->data[5], nmMailbox->data[6], nmMailbox->data[7]);
        e_darts_send_data(nmMailbox);
        //e_darts_send_signal(SU_MAILBOX_ACCEPT); //always respond accept for now for testing
    }
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
	    e_darts_print("pushing to queue at %x\n", thisCodeletQueue);
	    e_darts_print("queue head: %x, queue tail: %x\n", _dartsSUElements.darts_rt_codeletsQueue.headAddress, _dartsSUElements.darts_rt_codeletsQueue.tailAddress);
            toDecDep->codeletTemplate.codeletID = i;
            //while(pushCodeletQueue(thisCodeletQueue, &(toDecDep->codeletTemplate)) != CODELET_QUEUE_SUCCESS_OP);
	    int result = pushCodeletQueue(thisCodeletQueue, &(toDecDep->codeletTemplate));
	    e_darts_print("SU tried to push codelet to queue with result %d\n", result);
	    while(result != CODELET_QUEUE_SUCCESS_OP) {
	        if (result == CODELET_QUEUE_NOT_ENOUGH_SPACE) {
	            e_darts_print("SU codelet queue full (from SU decDep)\n");
                    //wait for a sec
		    int j = 0;
		    while(j<1000000*i) j++;
		}	
                result = pushCodeletQueue(thisCodeletQueue, &(toDecDep->codeletTemplate));
	    }
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
