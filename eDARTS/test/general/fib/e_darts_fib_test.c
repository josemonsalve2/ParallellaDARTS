#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "e-lib.h"
#include "codelet.h"
#include "codeletsQueue.h"
#include "threadedProcedure.h"
#include "tpClosuresQueue.h"
#include "e_darts_rt.h"
#include "e_darts_mailbox.h"
#include "e_DARTS.h"

extern codelet_t _dartsFinalCodelet;
extern nodeMailbox_t _dartsNodeMailbox;
void fibCheck();
void fibAdd();

    DEFINE_TP_MEM_REGIONS(fib_tp,
		              //DRAM
			      int number; // inputed number
			      int x; // result of first recursive call
			      int y; // result of second recursive call
			      int *result; // where to write back result
			      syncSlot_t *toDecDep; //address of syncSlot to decDep -- need this for cross-TP signaling
			      ,
			      //INTERNAL
			      ,
			      //DIST
		              ) 

    DEFINE_THREADED_PROCEDURE(fib_tp,2, {
			      memDRAM->number = number;
			      memDRAM->x = x;
			      memDRAM->y = y;
			      memDRAM->result = (int *) result;
			      memDRAM->toDecDep = (syncSlot_t *) toDecDep;
			      ASSIGN_SYNC_SLOT_CODELET(*this,0,fibCheck,1,1,1);
			      ASSIGN_SYNC_SLOT_CODELET(*this,1,fibAdd,2,2,1);
			      syncSlot_t *first_slot = GET_SYNC_SLOT(*this, 0);
			      e_darts_print("initializing TP\n");
			      DEC_DEP(first_slot);
		              }
			      , int number, int x, int y, int *result, syncSlot_t *toDecDep)

    DEFINE_TP_CLOSURE(fib_tp,int,int,int,int *,syncSlot_t *);

void pushFinalCodelet() {
    codeletsQueue_t * suCodeletQueue = (codeletsQueue_t *) DARTS_APPEND_COREID(0x808,&(_dartsCUElements.mySUElements->darts_rt_codeletsQueue));
    while (pushCodeletQueue(suCodeletQueue, &(_dartsFinalCodelet)) != CODELET_QUEUE_SUCCESS_OP);
}

void fibCheck()
{
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    fib_tp_memDRAM_t *memDRAM = (fib_tp_memDRAM_t *) actualTP->memDRAM;
    e_darts_print("fibCheck reports number as %d\n", memDRAM->number);
    if (memDRAM->number < 2) {
        *(memDRAM->result) = memDRAM->number;
        //syncSlot_t *add = GET_SYNC_SLOT(*actualTP, 1);
	if (memDRAM->toDecDep == NULL) {
            pushFinalCodelet();
        }
	else DEC_DEP(memDRAM->toDecDep);
    }
    else {
        syncSlot_t * next = (syncSlot_t *) DARTS_APPEND_COREID(0x808,GET_SYNC_SLOT(*actualTP, 1));
	e_darts_print("fibCheck's corresponding fibAdd is at %x\n", next);
        INVOKE(fib_tp, memDRAM->number - 1, 0, 0, &(memDRAM->x), next); //put x and y as 0, they'll be filled in
	INVOKE(fib_tp, memDRAM->number - 2, 0, 0, &(memDRAM->y), next);
    }
}
void fibAdd()
{
    e_darts_print("fibAdd running\n");
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    fib_tp_memDRAM_t *memDRAM = (fib_tp_memDRAM_t *) actualTP->memDRAM;
    *(memDRAM->result) = memDRAM->x + memDRAM->y;
    syncSlot_t *toDecDep = (syncSlot_t *) DARTS_APPEND_COREID(0x808,memDRAM->toDecDep);
    if (memDRAM->toDecDep == NULL) {
        e_darts_print("************** Result: %d **************\n", *(memDRAM->result));
        pushFinalCodelet();
    }
    else {
        e_darts_print("fibAdd DEC_DEPs on %x\n", toDecDep);
        DEC_DEP(toDecDep);
    }
    //decDep add of parent TP -- need parent TP address
    //for top level TP, that wouldn't work, need address of syncSlot to decDep
    //for top level TP, put address of syncSlot for termination codelet; 
    //i think in main if statement for SU you could just locally create one 
    //and pass that address to the parent TP invocation
    
}

int main(void)
{
    unsigned base0_0Address = (unsigned) e_get_global_address(0, 0, 0x0000);

    e_darts_cam_t CAM = {SU, CU, CU ,CU,\
	                 CU, CU, CU, CU,\
                         CU, CU, CU, CU,\
			 CU, CU, CU, CU};
    e_darts_rt(CAM, CU_ROUND_ROBIN, SU_ROUND_ROBIN);
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
        e_darts_node_mailbox_init();
        e_darts_print("Address of mailbox: %x\n", &(_dartsNodeMailbox));
	e_darts_print("Address of SUtoNM's data: %x\n", &(_dartsNodeMailbox.SUtoNM.data));
	e_darts_print("Address of SUtoNM's ack: %x\n", &(_dartsNodeMailbox.SUtoNM.ack));
	e_darts_print("Address of NMtoSU: %x\n", &(_dartsNodeMailbox.NMtoSU));
	e_darts_print("Address of NMtoSU's ack: %x\n", &(_dartsNodeMailbox.NMtoSU.ack));
        e_darts_print("message at start: %d\n", _dartsNodeMailbox.NMtoSU.signal);
	int localInt;
	syncSlot_t localSlot;
	syncSlot_t *finalSyncSlot = (syncSlot_t *) &localSlot;
        initSyncSlot(finalSyncSlot, 0, 1, 1, _dartsFinalCodelet, 1);
        e_darts_print("final codelet ID: %x, final syncSlot address: %x\n", finalSyncSlot->codeletTemplate.codeletID, finalSyncSlot);
        //INVOKE(fib_tp,2,0,0,(int *)DARTS_APPEND_COREID(0x808,&localInt),(syncSlot_t *)DARTS_APPEND_COREID(0x808,finalSyncSlot));
        INVOKE(fib_tp,7,0,0,(int *)DARTS_APPEND_COREID(0x808,&localInt),(syncSlot_t *) NULL);
    }
    e_darts_run();
    return 0;
}
