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
void empty1();
void empty2();
void empty3();
void empty4();
void empty5();
void empty6();
void read_message();

    DEFINE_TP_MEM_REGIONS(simple_tp,
		              //DRAM
			      int return_message;
			      ,
			      //INTERNAL
			      ,
			      //DIST
		              ) 

    DEFINE_THREADED_PROCEDURE(simple_tp,7, {
			      memDRAM->return_message = return_message;
			      ASSIGN_SYNC_SLOT_CODELET(*this,0,empty1,1,1,8);
			      ASSIGN_SYNC_SLOT_CODELET(*this,1,empty2,8,1,19);
			      ASSIGN_SYNC_SLOT_CODELET(*this,2,empty3,19,1,18);
			      ASSIGN_SYNC_SLOT_CODELET(*this,3,empty4,18,1,17);
			      ASSIGN_SYNC_SLOT_CODELET(*this,4,empty5,17,1,17);
			      ASSIGN_SYNC_SLOT_CODELET(*this,5,empty6,17,1,17);
			      //ASSIGN_SYNC_SLOT_CODELET(*this,6,read_message,3,1,1);
			      syncSlot_t* finalSyncSlot = GET_SYNC_SLOT(*this,6);
			      initSyncSlot(finalSyncSlot, 6, 1, 17, _dartsFinalCodelet, 1);
			      e_darts_print("final codelet ID: %x\n", finalSyncSlot->codeletTemplate.codeletID);
			      syncSlot_t *first_slot = GET_SYNC_SLOT(*this, 0);
			      e_darts_print("initializing TP\n");
			      DEC_DEP(first_slot);
		              }
			      , int return_message)

    DEFINE_TP_CLOSURE(simple_tp,int);

void empty1()
{
    e_darts_print("empty v1 executing\n");
    // signal read, empty func 
    unsigned x = 0;
    while (x < 10000000) x++;
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    syncSlot_t *next = GET_SYNC_SLOT(*actualTP, 1);
    DEC_DEP(next);
}
void empty2()
{
    e_darts_print("empty v2 executing\n");
    // signal read, empty func 
    unsigned x = 0;
    while (x < 10000000) x++;
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    syncSlot_t *next = GET_SYNC_SLOT(*actualTP, 2);
    DEC_DEP(next);
}
void empty3()
{
    e_darts_print("empty v3 executing\n");
    // signal read, empty func 
    unsigned x = 0;
    while (x < 10000000) x++;
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    syncSlot_t *next = GET_SYNC_SLOT(*actualTP, 3);
    DEC_DEP(next);
}
void empty4()
{
    e_darts_print("empty v4 executing\n");
    // signal read, empty func 
    unsigned x = 0;
    while (x < 10000000) x++;
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    syncSlot_t *next = GET_SYNC_SLOT(*actualTP, 4);
    DEC_DEP(next);
}
void empty5()
{
    e_darts_print("empty v5 executing\n");
    // signal read, empty func 
    unsigned x = 0;
    while (x < 10000000) x++;
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    syncSlot_t *next = GET_SYNC_SLOT(*actualTP, 5);
    DEC_DEP(next);
}
void empty6()
{
    e_darts_print("empty v6 executing\n");
    // signal read, empty func 
    unsigned x = 0;
    while (x < 10000000) x++;
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    syncSlot_t *next = GET_SYNC_SLOT(*actualTP, 6);
    DEC_DEP(next);
}

void read_message()
{
    e_darts_print("----------read executing\n");
    // read signal, check if its NM_REQUEST_SU_PROVIDE
    // if it is, get z from tpDataHeap and send that signal 
    // else, print incorrect
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    simple_tp_memDRAM_t *memDRAM = (simple_tp_memDRAM_t *) actualTP->memDRAM;
    //message signal_to_return = memDRAM->return_message;
    message signal_to_return = 2;
    message received = e_darts_receive_signal();
    e_darts_print("---------signal received: %d\n", received);
    if (received == 6) {
        e_darts_print("----------correct signal read\n");
        int result = e_darts_send_signal(&signal_to_return);
	e_darts_print("----------signal sent, function returned %d\n", result);
    }
    syncSlot_t *final = GET_SYNC_SLOT(*actualTP, 7);
    e_darts_print("final codelet signaled\n");
    DEC_DEP(final);
    // signal final codelet
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
        INVOKE(simple_tp,2);
    }
    e_darts_run();
    return 0;
}
