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
#define VECTOR_LENGTH 32 
#define CODELET_NUM 4

extern codelet_t _dartsFinalCodelet;
extern nodeMailbox_t _dartsNodeMailbox;
void intVectorAdd();
void intReportResult();
void floatVectorAdd();
void floatReportResult();
void doubleVectorAdd();
void doubleReportResult();
void fillArray(float *array, float seed);

typedef struct int_vectors_s {
    int x[VECTOR_LENGTH];
    int y[VECTOR_LENGTH];
    int result[VECTOR_LENGTH];
} int_vectors_t;

typedef struct float_vectors_s {
    float x[VECTOR_LENGTH];
    float y[VECTOR_LENGTH];
    float result[VECTOR_LENGTH];
} float_vectors_t;

typedef struct double_vectors_s {
    double x[VECTOR_LENGTH];
    double y[VECTOR_LENGTH];
    double result[VECTOR_LENGTH];
} double_vectors_t;

int_vectors_t global_int_vectors;
float_vectors_t global_float_vectors;
double_vectors_t global_double_vectors;

    DEFINE_TP_MEM_REGIONS(int_vector_add_tp,
                      //DRAM
                  int_vectors_t internal_data;
                  ,
                  //INTERNAL
                  ,
                  //DIST
                      ) 

    DEFINE_THREADED_PROCEDURE(int_vector_add_tp,3, {
                      memDRAM->internal_data = internal_data;
                  /*
                  e_darts_print("int vector x: [ ");
                  for (int i=0; i<VECTOR_LENGTH; i++) {
                                  e_darts_print("%d ", memDRAM->internal_data.x[i]);
                  }
                  e_darts_print("]\n");
                  e_darts_print("int vector y: [ ");
                  for (int i=0; i<VECTOR_LENGTH; i++) {
                                  e_darts_print("%d ", memDRAM->internal_data.y[i]);
                  }
                  e_darts_print("]\n");
                  */
                  //fillArray(memDRAM->internal_data.x, 8.0);
                  //fillArray(memDRAM->internal_data.y, 1.4);
                  e_darts_print("filling arrays \n");
                  for (int i=0; i<VECTOR_LENGTH; i++) {
                                  //memDRAM->internal_data.x[i] = (float) (i * 8.3 - i);
                                  memDRAM->internal_data.x[i] = i;
                                  //memDRAM->internal_data.y[i] = (float) (i * 1.4 - i);
                                  memDRAM->internal_data.y[i] = i%2;
                  }
                  ASSIGN_SYNC_SLOT_CODELET(*this,0,intVectorAdd,1,1,CODELET_NUM);
                  ASSIGN_SYNC_SLOT_CODELET(*this,1,intReportResult,CODELET_NUM,1,1);
                  syncSlot_t *first_slot = GET_SYNC_SLOT(*this, 0);
                  syncSlot_t *finalSyncSlot = GET_SYNC_SLOT(*this, 2);
                  initSyncSlot(finalSyncSlot, 2, 1, 1, _dartsFinalCodelet, 1);
                  e_darts_print("initializing TP\n");
                  DEC_DEP(first_slot);
                      }
                  , int_vectors_t internal_data)

    DEFINE_TP_CLOSURE(int_vector_add_tp, int_vectors_t);


    DEFINE_TP_MEM_REGIONS(float_vector_add_tp,
                      //DRAM
                  float_vectors_t internal_data;
                  ,
                  //INTERNAL
                  ,
                  //DIST
                      ) 

    DEFINE_THREADED_PROCEDURE(float_vector_add_tp,3, {
                      memDRAM->internal_data = internal_data;
                  /*
                  e_darts_print("internal_data at %x\n", &(memDRAM->internal_data));
                  e_darts_print("x at %x\n", &(memDRAM->internal_data.x));
                  e_darts_print("x[1] at %x\n", &(memDRAM->internal_data.x[1]));
                  e_darts_print("y at %x\n", memDRAM->internal_data.y);
                  e_darts_print("result at %x\n", memDRAM->internal_data.result);
                  e_darts_print("int vector x: [ ");
                  for (int i=0; i<VECTOR_LENGTH; i++) {
                                  e_darts_print("%f ", memDRAM->internal_data.x[i]);
                  }
                  e_darts_print("]\n");
                  e_darts_print("int vector y: [ ");
                  for (int i=0; i<VECTOR_LENGTH; i++) {
                                  e_darts_print("%f ", memDRAM->internal_data.y[i]);
                  }
                  e_darts_print("]\n");
                  //fillArray(memDRAM->internal_data.x, 8.0);
                  //fillArray(memDRAM->internal_data.y, 1.4);
                  */
                  e_darts_print("filling arrays \n");
                  for (int i=0; i<VECTOR_LENGTH; i++) {
                                  //memDRAM->internal_data.x[i] = (float) (i * 8.3 - i);
                                  memDRAM->internal_data.x[i] = (float) i;
                                  //memDRAM->internal_data.y[i] = (float) (i * 1.4 - i);
                                  memDRAM->internal_data.y[i] = (float) 0.0;
                  }
                  //e_darts_print("vector x: [ %f %f %f %f ]\n", 0, memDRAM->internal_data.x[0], memDRAM->internal_data.x[1], memDRAM->internal_data.x[2], memDRAM->internal_data.x[3]);
                  //e_darts_print("vector y: [ %f %f %f %f ]\n", 0, memDRAM->internal_data.y[0], memDRAM->internal_data.y[1], memDRAM->internal_data.y[2], memDRAM->internal_data.y[3]);
                  ASSIGN_SYNC_SLOT_CODELET(*this,0,floatVectorAdd,1,1,CODELET_NUM);
                  ASSIGN_SYNC_SLOT_CODELET(*this,1,floatReportResult,CODELET_NUM,1,1);
                  syncSlot_t *first_slot = GET_SYNC_SLOT(*this, 0);
                  syncSlot_t *finalSyncSlot = GET_SYNC_SLOT(*this, 2);
                  initSyncSlot(finalSyncSlot, 2, 1, 1, _dartsFinalCodelet, 1);
                  e_darts_print("initializing TP\n");
                  DEC_DEP(first_slot);
                      }
                  , float_vectors_t internal_data)

    DEFINE_TP_CLOSURE(float_vector_add_tp, float_vectors_t);


void intVectorAdd()
{
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    codelet_t *thisCodelet = (codelet_t *) _dartsCUElements.currentCodelet;
    int codeletID = (int) thisCodelet->codeletID;
    int_vector_add_tp_memDRAM_t *memDRAM = (int_vector_add_tp_memDRAM_t *) actualTP->memDRAM;
    int *x_DRAM = (int *) &(memDRAM->internal_data.x);
    int *y_DRAM = (int *) &(memDRAM->internal_data.y);
    int *result_DRAM = (int *) &(memDRAM->internal_data.result);
    // chunk by codelet ID
    for (int i=VECTOR_LENGTH/CODELET_NUM*codeletID; i<VECTOR_LENGTH/CODELET_NUM*(codeletID+1); i++) {
        memDRAM->internal_data.result[i] = memDRAM->internal_data.x[i] + memDRAM->internal_data.y[i];
    }
    syncSlot_t *reportSyncSlot = (syncSlot_t *) GET_SYNC_SLOT(*actualTP, 1);
    DEC_DEP(reportSyncSlot);
}

void intReportResult()
{
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    int_vector_add_tp_memDRAM_t *memDRAM = (int_vector_add_tp_memDRAM_t *) actualTP->memDRAM;
    int *result_DRAM = (int *) &(memDRAM->internal_data.result);
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
    e_darts_print("result: [ ");
    for (int i=0; i<VECTOR_LENGTH; i++) {
        e_darts_print("%d ", memDRAM->internal_data.result[i]); 
    }
    e_darts_print(" ]\n");
    /*
    syncSlot_t *finalSyncSlot = (syncSlot_t *) GET_SYNC_SLOT(*actualTP,2);
    DEC_DEP(finalSyncSlot);
    */
    INVOKE(float_vector_add_tp,global_float_vectors);
}

void floatVectorAdd()
{
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    codelet_t *thisCodelet = (codelet_t *) _dartsCUElements.currentCodelet;
    int codeletID = (int) thisCodelet->codeletID;
    float_vector_add_tp_memDRAM_t *memDRAM = (float_vector_add_tp_memDRAM_t *) actualTP->memDRAM;
    float *x_DRAM = (float *) &(memDRAM->internal_data.x);
    float *y_DRAM = (float *) &(memDRAM->internal_data.y);
    float *result_DRAM = (float *) &(memDRAM->internal_data.result);
    // chunk by codelet ID
    for (int i=VECTOR_LENGTH/CODELET_NUM*codeletID; i<VECTOR_LENGTH/CODELET_NUM*(codeletID+1); i++) {
        result_DRAM[i] = x_DRAM[i] + y_DRAM[i];
    }
    syncSlot_t *reportSyncSlot = (syncSlot_t *) GET_SYNC_SLOT(*actualTP, 1);
    DEC_DEP(reportSyncSlot);
}

void floatReportResult()
{
    _tp_metadata_t *actualTP = (_tp_metadata_t *) _dartsCUElements.currentThreadedProcedure;
    float_vector_add_tp_memDRAM_t *memDRAM = (float_vector_add_tp_memDRAM_t *) actualTP->memDRAM;
    e_darts_print("vector x: [ ");
    for (int i=0; i<VECTOR_LENGTH; i++) {
        e_darts_print("%f ", memDRAM->internal_data.x[i]);
    }
    e_darts_print("]\n");
    e_darts_print("vector y: [ ");
    for (int i=0; i<VECTOR_LENGTH; i++) {
        e_darts_print("%f ", memDRAM->internal_data.y[i]);
    }
    e_darts_print("]\n");
    float *result_DRAM = (float *) &(memDRAM->internal_data.result);
    e_darts_print("result: [ ");
    for (int i=0; i<VECTOR_LENGTH; i++) {
        e_darts_print("%f ", 0, result_DRAM[i]); 
    }
    e_darts_print(" ]\n");
    syncSlot_t *finalSyncSlot = (syncSlot_t *) GET_SYNC_SLOT(*actualTP,2);
    DEC_DEP(finalSyncSlot);
}

void copyArray(float *array1, float *array2);

void fillArray(float *array, float seed)
{
    array = (float *) DARTS_APPEND_COREID(0x80e, array);
    for (int i=0; i<VECTOR_LENGTH; i++) {
        array[i] = (float) (i * seed - i);
    }
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
    e_darts_print("size of double_vectors_t: %d\n", sizeof(double_vectors_t));
        INVOKE(int_vector_add_tp,global_int_vectors);
    }
    e_darts_run();
    return 0;
}
