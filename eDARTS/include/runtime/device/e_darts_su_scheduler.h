/**
 * @file e_darts_su_scheduler.h
 * @author Jose M Monsalve
 * @date 7 August 2017
 * @brief SU scheduler definitions
 * @todo Add copyright
 * @todo add printing of variables
 *
 * Definition of the SU scheduler function
 *
 */
#ifndef _E_DARTS_SU_SCHEDULER_H_
#define _E_DARTS_SU_SCHEDULER_H_

#include "syncSlot.h"
#include "codelet.h"
#include "threadedProcedure.h"
#include "e_darts_scheduler_base.h"
#include "e_darts_global.h"
#include "e_darts_su.h"

typedef enum {
    SU_ROUND_ROBIN
} su_scheduler_selector;

void darts_set_su_scheduler(scheduler_t* su_scheduler, su_scheduler_selector su_scheduler_policy);

#endif /* _E_DARTS_SU_SCHEDULER_H_ */
