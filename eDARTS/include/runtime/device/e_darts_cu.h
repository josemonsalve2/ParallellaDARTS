/**
 * @file e_darts_cu.h
 * @author Jose M Monsalve
 * @date 8 July 2017
 * @brief CU definitions
 * @todo Add copyright
 * @todo add printing of variables
 *
 * This file defines all the required symbols that are mapped into the CUs
 * memory. This is, this file has the runtime elements and functions for the
 * CU.
 *
 */

#ifndef _E_DARTS_CU_H_
#define _E_DARTS_CU_H_

#include "codeletsQueue.h"
#include "codelet.h"
#include "syncSlot.h"
#include "darts_rt_params.h"
#include "threadedProcedure.h"
#include "e_darts_cu_scheduler.h"
#include "e_darts_cam.h"

typedef struct __attribute__ ((__packed__)) su_runtime_elements_s su_runtime_elements_t;

/*
 * @brief CU memory structure struct
 *
 * This struct is used to organize and obtain information regarding the CU.
 * This includes required elements:
 * * scheduler: contains the scheduling policies and runtime APIs implementations
 * * codeletsQueue_t: metadata of the codelet queue
 * * padding for the queue itself
 * * syncronization slots
 *
 *
 */
typedef struct __attribute__ ((__packed__)) cu_runtime_elements_s {
    _tp_metadata_t * currentThreadedProcedure;
    codelet_t * currentCodelet;
    codeletsQueue_t darts_rt_codeletsQueue;
    char codeletsQueuePadding[_DARTS_CODELET_QUEUE_SIZE_BYTES]; // Must be after codeletsQueue_t
    syncSlot_t syncSlots[_DARTS_NUM_SYNC_SLOTS_PER_CU];
    su_runtime_elements_t * mySUElements; // SET THIS WITH FULL ADDRESS WITH COREID
} cu_runtime_elements_t;

extern cu_runtime_elements_t _dartsCUElements;

/* @brief CU runtime function
 *
 * This is the runtime function that will be loaded within the CU
 * This function is the one that starts the scheduling policy
 * The scheduling policy must use the above definitions
 *
 */
void _CU_rt();

#endif /* _E_DARTS_CU_H_ */
