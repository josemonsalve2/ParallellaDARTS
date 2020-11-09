#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "codelet.h"
#include "codeletsQueue.h"
#include "threadedProcedure.h"
#include "tpClosuresQueue.h"
#include "e_darts_rt.h"
#include "e_DARTS.h"

extern codelet_t _dartsFinalCodelet;
void add();
void read();

    DEFINE_TP_MEM_REGIONS(simple_tp,
		              //DRAM
			      int z;
			      ,
			      //INTERNAL
			      ,
			      //DIST
		              ) 

    DEFINE_THREADED_PROCEDURE(simple_tp,3, {
                              //e_darts_print("Initializing simple TP at %x\n", this);
			      memDRAM->z = z;
			      //e_darts_print("syncSlot 0 at %x\n", GET_SYNC_SLOT(*this,0));
			      //e_darts_print("syncSlot 1 at %x\n", GET_SYNC_SLOT(*this,1));
			      //e_darts_print("syncSlot 2 at %x\n", GET_SYNC_SLOT(*this,2));
			      ASSIGN_SYNC_SLOT_CODELET(*this,0,add,1,1,3);
			      //syncSlot_t* first = GET_SYNC_SLOT(*this,0);
			      //first->tpFrame = (_tp_metadata_t *) DARTS_APPEND_COREID(0x808,this);
			      //e_darts_print("syncSlot 0 now has tpFrame -> %x\n", first->tpFrame);
			      ASSIGN_SYNC_SLOT_CODELET(*this,1,read,3,1,1);
			      //e_darts_print("Assigned syncSlots for add and read\n");
			      //ASSIGN_SYNC_SLOT_CODELET(*this,2,terminationCodelet,1,1,1);
			      syncSlot_t* finalSyncSlot = GET_SYNC_SLOT(*this,2);
			      initSyncSlot(finalSyncSlot, 2, 1, 1, _dartsFinalCodelet, 1);
			      //e_darts_print("initialized final syncSlot\n");
			      syncSlot_t *addCodelet = GET_SYNC_SLOT(*this, 0);
			      //e_darts_print("dec_dep add at %x\n", (unsigned)addCodelet);
			      DEC_DEP(addCodelet);
			      //syncSlotDecDep(addCodelet);
			      //e_darts_print("DEC_DEP called on addCodelet\n");
		              }
			      , int z)
                              //)
    /*
    DEFINE_THREADED_PROCEDURE(simple_tp,2, {
		              e_darts_print("Initializing simple TP\n");
			      ASSIGN_SYNC_SLOT_CODELET(*this,0,add,1,1,4);
			      e_darts_print("Assigned syncSlot for add\n");
			      syncSlot_t* finalSyncSlot = GET_SYNC_SLOT(*this,1);
			      initSyncSlot(finalSyncSlot, 1, 1, 4, _dartsFinalCodelet, 1);
			      syncSlot_t *addCodelet = GET_SYNC_SLOT(*this, 0);
			      DEC_DEP(addCodelet);
			      e_darts_print("DEC_DEP done\n");
			      })
    */

    DEFINE_TP_CLOSURE(simple_tp,int);
    //DEFINE_TP_CLOSURE(simple_tp);

void add()
{
    //where does start codelet come in?
    e_darts_print(" ADD EXECUTING \n");
    int x = 5;
    int y = 2;
    int z = y + x;
    e_darts_print("\tRESULT: %d\n", z);
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    //how to get TP frame from inside codelet fire function?
    //_tp_metadata_t *actualTP = (_tp_metadata_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsCUElements.currentThreadedProcedure));
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    simple_tp_memDRAM_t *memDRAM = (simple_tp_memDRAM_t *) actualTP->memDRAM;
    e_darts_print("memDRAM for add located at %x\n", memDRAM);
    memDRAM->z = memDRAM->z + z; //should be 7
    //e_darts_print("actual TP from add codelet at %x\n", actualTP);
    syncSlot_t *next = GET_SYNC_SLOT(*actualTP, 1);
    //e_darts_print("dec_dep read at %x\n", (unsigned)next);
    DEC_DEP(next);
    e_darts_print(" SIGNALED ADD2 \n");
}

void read()
{
    e_darts_print(" ADD 2 EXECUTING \n");
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    simple_tp_memDRAM_t *memDRAM = (simple_tp_memDRAM_t *) actualTP->memDRAM;
    e_darts_print("memDRAM for read located at %x\n", memDRAM);
    int read_z = memDRAM->z;
    e_darts_print(" value read: %d\n", read_z);
    int new_z = read_z - 6;
    e_darts_print("\tRESULT: %d\n", new_z);
    memDRAM->z = new_z;
    syncSlot_t *next = GET_SYNC_SLOT(*actualTP, 2);
    DEC_DEP(next);
}

int main(void)
{
    unsigned base0_0Address = (unsigned) e_get_global_address(0, 0, 0x0000);

    e_darts_cam_t CAM;
    e_darts_rt(CAM, CU_ROUND_ROBIN, SU_ROUND_ROBIN);
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
        INVOKE(simple_tp,0);
    }
    e_darts_run();
    //e_darts_run(simple_tp.baseClosure); //see src code, doesn't seem to actually push before going into policy
    //INVOKE(simple_tp, 8);
    //INVOKE(simple_tp);
    //dartsRtScheduler.policy();
    return 0;
}
