/**
 * @file e_darts_su.h
 * @author Jose M Monsalve
 * @date 8 July 2017
 * @brief SU definitions
 * @todo Add copyright
 * @todo add printing of variables
 *
 * This file defines all the required symbols that are mapped into the SUs
 * memory. This is, this file has the runtime elements and functions for the
 * SU.
 *
 */

#ifndef _E_DARTS_SU_H_
#define _E_DARTS_SU_H_

#include "codeletsQueue.h"
#include "tpClosuresQueue.h"
#include "codelet.h"
#include "syncSlot.h"
#include "darts_rt_params.h"
#include "threadedProcedure.h"
#include "e_darts_su_scheduler.h"
#include "e_darts_cu.h"

typedef struct __attribute__ ((__packed__)) su_runtime_elements_s {
    _tp_metadata_t * currentThreadedProcedure;
    codelet_t * currentCodelet;
    syncSlot_t syncSlots[_DARTS_NUM_SYNC_SLOTS_PER_SU];
    codeletsQueue_t darts_rt_codeletsQueue;
    char codeletsQueuePadding[_DARTS_CODELET_QUEUE_SIZE_BYTES]; // Must be after codeletsQueue_t
    tpClosuresQueue_t darts_rt_tpclosuresQueue;
    char tpClosuresQueuePadding[_DARTS_TPCLOSURES_QUEUE_SIZE_BYTES]; // must be after tpClosuresQueue_t
    cu_runtime_elements_t * myCUElements[_DARTS_NUM_CORES]; // SET THIS WITH FULL ADDRESS WITH COREID
} su_runtime_elements_t;

typedef struct __attribute__ ((__packed__)) tp_heap_space_s {
    char tpHeapPadding[_DARTS_TP_HEAP_SIZE_BYTES];
} tp_heap_space_t;

typedef struct __attribute__ ((__packed__)) tp_data_heap_space_s {
    char tpHeapPadding[_DARTS_TP_DATA_HEAP_SIZE_BYTES];
} tp_data_heap_space_t;

typedef struct __attribute__ ((__packed__)) tp_data_heap_DRAM_space_s {
    char tpHeapPadding[_DARTS_TP_DATA_HEAP_DRAM_SIZE_BYTES];
} tp_data_heap_DRAM_space_t;

extern su_runtime_elements_t _dartsSUElements;
extern tp_heap_space_t _dartsTpHeap; //for syncSlots & metadata - in SU
extern tp_data_heap_space_t _tpDataHeap; //on SU, local data
extern tp_data_heap_DRAM_space_t _tpDataHeapDRAM; // DRAM data

/* @brief SU runtime function
 *
 * This is the runtime function that will be loaded within the SU
 * This function is the one that starts the scheduling policy
 * The scheduling policy must use the above definitions
 *
 */
void _SU_rt();

#endif /* _E_DARTS_SU_H_ */

