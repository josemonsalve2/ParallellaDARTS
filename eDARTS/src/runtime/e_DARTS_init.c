#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "codelet.h"
#include "codeletsQueue.h"
#include "e_DARTS.h"


/*
 * This is a testing function with no dependencies.
 *
 */
void sum()
{
    unsigned *value;
    value = (unsigned *) 0x2228;
    *value = 999;
}

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
    _tp_metadata_t *actualTP = (_tp_metadata_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsCUElements->currentThreadedProcedure));
    //simple_tp_memDRAM_t *memDRAM = (simple_tp_memDRAM_t *) actualTP->memDRAM;
    //memDRAM->z = z;
    syncSlot_t *next = GET_SYNC_SLOT(*actualTP, 1);
    syncSlotDecDep(next);
    e_darts_print(" SIGNALED READ \n");
}

void read()
{
    e_darts_print(" ADD 2 EXECUTING \n");
    int read_z = 7;
    int new_z = 7 - 6;
    e_darts_print("\tRESULT: %d\n", new_z);
    unsigned thisCoreID;
    DARTS_GETCOREID(thisCoreID);
    _tp_metadata_t *actualTP = (_tp_metadata_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsCUElements->currentThreadedProcedure));
    //simple_tp_memDRAM_t *memDRAM = (simple_tp_memDRAM_t *) actualTP->memDRAM;
    //read_z = memDRAM->z = z;
    
    e_darts_print(" value read: %d\n", read_z);
}

int main(void)
{
    unsigned *cd;
    cd = 0x4000;
    *cd = (unsigned) &sum;

    DEFINE_TP_MEM_REGIONS(simple_tp,
		              //DRAM
			      int z
			      ,
			      //INTERNAL
			      ,
			      //DIST
		              ) 

    DEFINE_THREADED_PROCEDURE(simple_tp,2, {
                              e_darts_print("Initializing simple TP \n");
			      memDRAM->z = z;
			      ASSIGN_SYNC_SLOT_CODELET(*this,0,add,1,1,1);
			      ASSIGN_SYNC_SLOT_CODELET(*this,1,read,1,1,1);
			      //what to do with final codelet? how to make sure all cures 
			      //set darts_rt_alive = 0? If it just comes last in the TP, only 1 CU
			      //will turn off which is obviously not the intended effect
			      ASSIGN_SYNC_SLOT_CODELET(*this,2,terminationCodelet,1,1,1);
			      syncSlot_t *addCodelet = GET_SYNC_SLOT(*this, 1);
			      syncSlotDecDep(addCodelet);
		              }
			      , int z)

    DEFINE_TP_CLOSURE(simple_tp,int);

    e_darts_cam_t CAM;
    e_darts_rt(CAM, CU_ROUND_ROBIN, SU_ROUND_ROBIN);
    e_darts_run(simple_tp); //see src code, doesn't seem to actually push before going into policy
    return 0;
}
