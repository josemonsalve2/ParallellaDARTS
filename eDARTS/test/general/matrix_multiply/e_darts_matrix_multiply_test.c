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


#define N 6

extern codelet_t _dartsFinalCodelet;
extern nodeMailbox_t _dartsNodeMailbox;
void computeElement();
void printResult();

typedef struct int_matrix_s {
    int data[N*N];
} int_matrix_t;

int_matrix_t x_in;
int_matrix_t y_in;
int_matrix_t mat_out;

    DEFINE_TP_MEM_REGIONS(mat_mult_tp,
		              //DRAM
			      int_matrix_t x;
			      int_matrix_t y;
			      int_matrix_t out;
			      ,
			      //INTERNAL
			      ,
			      //DIST
		              ) 

    DEFINE_THREADED_PROCEDURE(mat_mult_tp,3, {
			      memDRAM->x = x;
			      e_darts_print("memDRAM->x: %x\n", memDRAM->x);
			      memDRAM->y = y;
			      memDRAM->out = out;
			      e_darts_print("x:\n");
                              for (int i=0; i<N; i++) {
                                  e_darts_print("%d\t%d\t%d\t%d\t%d\t%d\n", memDRAM->x.data[i*N],memDRAM->x.data[i*N+1],memDRAM->x.data[i*N+2],memDRAM->x.data[i*N+3], memDRAM->x.data[i*N+4], memDRAM->x.data[i*N+5]);
                              }
			      e_darts_print("y:\n");
                              for (int i=0; i<N; i++) {
                                  e_darts_print("%d\t%d\t%d\t%d\t%d\t%d\n", memDRAM->y.data[i*N],memDRAM->y.data[i*N+1],memDRAM->y.data[i*N+2],memDRAM->y.data[i*N+3], memDRAM->y.data[i*N+4], memDRAM->y.data[i*N+5]);
                              }
			      //start with one codelet per element in result matrix
			      ASSIGN_SYNC_SLOT_CODELET(*this,0,computeElement,1,1,N*N);
			      ASSIGN_SYNC_SLOT_CODELET(*this,1,printResult,N*N,N*N,1);
			      syncSlot_t* finalSyncSlot = GET_SYNC_SLOT(*this,2);
                              initSyncSlot(finalSyncSlot, 2, 1, 1, _dartsFinalCodelet, 1);
			      syncSlot_t *first_slot = GET_SYNC_SLOT(*this, 0);
			      e_darts_print("initializing TP\n");
			      DEC_DEP(first_slot);
			      e_darts_print("first syncSlot has ID %d, currentDep %d, tpFrame at %x, and %d codelets\n", first_slot->slotID, first_slot->currentDep, first_slot->tpFrame, first_slot->numCodelets);
		              }
			      , int_matrix_t x, int_matrix_t y, int_matrix_t out)

    DEFINE_TP_CLOSURE(mat_mult_tp,int_matrix_t,int_matrix_t,int_matrix_t);

void computeElement()
{
    e_darts_print("computeElement running\n");
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    mat_mult_tp_memDRAM_t *memDRAM = (mat_mult_tp_memDRAM_t *) actualTP->memDRAM;
    codelet_t *thisCodelet = (codelet_t *) _dartsCUElements.currentCodelet;
    int codeletID = (int) thisCodelet->codeletID;
    int row = (int) codeletID / N;
    int col = (int) codeletID % N;
    int acc = 0;
    //perform multiplication
    for (int i=0; i<N; i++) {
        acc += memDRAM->x.data[row*N+i] * memDRAM->y.data[i*N+col];
    }
    memDRAM->out.data[row*N+col] = acc;
    syncSlot_t *printSlot = (syncSlot_t *) GET_SYNC_SLOT(*actualTP, 1);
    DEC_DEP(printSlot);
}

void computeElementTiled()
{
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    mat_mult_tp_memDRAM_t *memDRAM = (mat_mult_tp_memDRAM_t *) actualTP->memDRAM;
    codelet_t *thisCodelet = (codelet_t *) _dartsCUElements.currentCodelet;
    int codeletID = (int) thisCodelet->codeletID;
    int row = (int) codeletID / N;
    int col = (int) codeletID % N;
    int acc = 0;
    //perform multiplication
    for (int i=0; i<N; i++) {
        acc += memDRAM->x.data[row*N+i] * memDRAM->y.data[i*N+col];
    }
    memDRAM->out.data[row*N+col] = acc;
    syncSlot_t *printSlot = (syncSlot_t *) GET_SYNC_SLOT(*actualTP, 1);
    DEC_DEP(printSlot);
}

void printResult()
{
    e_darts_print("printResult running\n");
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    mat_mult_tp_memDRAM_t *memDRAM = (mat_mult_tp_memDRAM_t *) actualTP->memDRAM;
    //e_darts_print("memDRAM: %x\n", memDRAM);
    //e_darts_print("memDRAM->x.data: %x\n", memDRAM->x.data);
    int *data_pointer = memDRAM->x.data;
    //e_darts_print("data pointer: %x\n", data_pointer);
    int *result = (int *) memDRAM->out.data;
    //e_darts_print("Result: \n");
    for (int i=0; i<N; i++) {
        e_darts_print("%d\t%d\t%d\t%d\t%d\t%d\n", memDRAM->out.data[i*N],memDRAM->out.data[i*N+1],memDRAM->out.data[i*N+2],memDRAM->out.data[i*N+3], memDRAM->out.data[i*N+4], memDRAM->out.data[i*N+5]);
    }
    syncSlot_t *finalSyncSlot = (syncSlot_t *) GET_SYNC_SLOT(*actualTP, 2);
    DEC_DEP(finalSyncSlot);
}

int main(void)
{
    e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX); //sets timer zero to max value since its going to count downwards
    e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK); //start timer zero to record clock cycles (decrements on event)
    unsigned base0_0Address = (unsigned) e_get_global_address(0, 0, 0x0000);

    e_darts_cam_t CAM = {SU, CU, CU ,CU,\
	                 CU, CU, CU, CU,\
                         CU, CU, CU, CU,\
			 CU, CU, CU, CU};
    e_darts_rt(CAM, CU_ROUND_ROBIN, SU_ROUND_ROBIN);
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
	for (int i=0; i<N*N; i++) {
            x_in.data[i] = i;
	    y_in.data[i] = (i * 3) % 5;
	}
        INVOKE(mat_mult_tp,x_in,y_in,mat_out);
    }
    e_darts_run();
    unsigned clock_cycles = E_CTIMER_MAX - e_ctimer_stop(E_CTIMER_0);
    int row = e_group_config.core_row;
    int col = e_group_config.core_col;
    e_darts_print("core (%d,%d) reports %d cycles\n", row, col, clock_cycles);
    return 0;
}
