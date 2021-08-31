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

// VECTOR_LENGTH should be divisible by CODELET_NUM for this example
#define VECTOR_LENGTH 16
#define CODELET_NUM 4

extern codelet_t _dartsFinalCodelet;
extern nodeMailbox_t _dartsNodeMailbox;
void intVectorDotProduct();
void intReportResult();

typedef struct int_vectors_s {
    int x[VECTOR_LENGTH];
    int y[VECTOR_LENGTH];
} int_vectors_t;


int_vectors_t global_int_vectors;
/* ------------------ INTEGER VECTOR_DOT TP ------------------*/
DEFINE_TP_MEM_REGIONS(int_vector_dotProduct_tp,
    //DRAM
    int_vectors_t internal_data;
    int result[CODELET_NUM];
    int final_result;
    ,
    //INTERNAL
    int test;
    ,
    //DIST
)
DEFINE_THREADED_PROCEDURE_ARGS(int_vector_dotProduct_tp,
    //Syncslots number
    3,
    //Initialization function
    {
        memLocal->test = 0;
        //memDRAM->internal_data = internal_data;
        e_darts_print("Filling arrays \n");
        for (int i=0; i<VECTOR_LENGTH; i++) {
            memDRAM->internal_data.x[i] = i;
            memDRAM->internal_data.y[i] = VECTOR_LENGTH - i;
        }
        // ID=0, Function= intVectorDotProduct(), DepsNum=1, InitialDep=1, CodeletsNum=CODELET_NUM
        ASSIGN_SYNC_SLOT_CODELET(*this, 0, intVectorDotProduct, 1, 1, CODELET_NUM);
        // ID=0, Function= intReportResult(), DepsNum=3, InitialDep=1, CodeletsNum=1
        ASSIGN_SYNC_SLOT_CODELET(*this, 1, intReportResult, CODELET_NUM, 1, 1);
        // ----
        syncSlot_t *first_slot = GET_SYNC_SLOT(*this, 0);
        syncSlot_t *finalSyncSlot = GET_SYNC_SLOT(*this, 2);
        initSyncSlot(finalSyncSlot, 2, 1, 1, _dartsFinalCodelet, 1);
        e_darts_print("Initializing TP\n");
        DEC_DEP(first_slot);
    }
    //Initialization function parameters
    //int_vectors_t internal_data
)
//DEFINE_TP_CLOSURE(int_vector_dotProduct_tp, int_vectors_t);
DEFINE_TP_CLOSURE(int_vector_dotProduct_tp);

void intVectorDotProduct() {
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    codelet_t *thisCodelet = (codelet_t *) _dartsCUElements.currentCodelet;
    int codeletID = (int) thisCodelet->codeletID;
    int_vector_dotProduct_tp_memDRAM_t *memDRAM = (int_vector_dotProduct_tp_memDRAM_t *) actualTP->memDRAM;
    // chunk by codelet ID
    memDRAM->result[codeletID] = 0;
    for (int i=VECTOR_LENGTH/CODELET_NUM*codeletID; i<VECTOR_LENGTH/CODELET_NUM*(codeletID+1); i++) {
        memDRAM->result[codeletID] += memDRAM->internal_data.x[i] * memDRAM->internal_data.y[i];
    }
    syncSlot_t *reportSyncSlot = (syncSlot_t *) GET_SYNC_SLOT(*actualTP, 1);
    DEC_DEP(reportSyncSlot);
}

void intReportResult() {
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    int_vector_dotProduct_tp_memDRAM_t *memDRAM = (int_vector_dotProduct_tp_memDRAM_t *) actualTP->memDRAM;
    //Sum of results
    memDRAM->final_result = 0;
    for (int i=0; i<CODELET_NUM; i++)
        memDRAM->final_result += memDRAM->result[i];
    
    //Report
    e_darts_print("intReportResult:\n");
    e_darts_print("vector x: [ ");
    for (int i=0; i<VECTOR_LENGTH; i++) {
        e_darts_print("%d ", memDRAM->internal_data.x[i]);
    }
    e_darts_print("]\n");
    e_darts_print("vector y: [ ");
    for (int i=0; i<VECTOR_LENGTH; i++) {
        e_darts_print("%d ", memDRAM->internal_data.y[i]);
    }
    e_darts_print("]\n");
    e_darts_print("result: %d", memDRAM->final_result);
    //End process
    syncSlot_t *finalSyncSlot = (syncSlot_t *) GET_SYNC_SLOT(*actualTP,2);
    DEC_DEP(finalSyncSlot);
}

/* ------------------ MAIN FUNCTION ------------------*/
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
        INVOKE(int_vector_dotProduct_tp, global_int_vectors);
    }
    e_darts_run();
    return 0;
}
