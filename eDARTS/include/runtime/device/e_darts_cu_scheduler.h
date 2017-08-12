/**
 * @file e_darts_cu_scheduler.h
 * @author Jose M Monsalve
 * @date 7 August 2017
 * @brief CU definitions
 * @todo Add copyright
 * @todo add printing of variables
 *
 * Definition of the CU scheduler function
 *
 */
#ifndef _E_DARTS_CU_SCHEDULER_H_
#define _E_DARTS_CU_SCHEDULER_H_

#include "syncSlot.h"
#include "codelet.h"
#include "threadedProcedure.h"
#include "tpClosuresQueue.h"
#include "e_darts_scheduler_base.h"
#include "e_darts_global.h"
#include "e_darts_cu.h"

typedef enum {
    CU_ROUND_ROBIN
} cu_scheduler_selector;

void darts_set_cu_scheduler(scheduler_t* cu_scheduler, cu_scheduler_selector cu_scheduler_policy);

#endif /* _E_DARTS_CU_SCHEDULER_H_ */
