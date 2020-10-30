/**
 * @file e_darts_rt.h
 * @author Jose M Monsalve
 * @date 7 August 2017
 * @brief runtime functions for darts.
 * @todo Add copyright
 *
 * This file provides the runtime functions that the user will use
 * to define and start the runtime. Additionally, any definition that is
 * required by the runtime to work (like the final codelet) are also defined
 * here
 */
#include "e_darts_cam.h"
#include "e_darts_cu.h"
#include "e_darts_su.h"
#include "e_darts_global.h"
#include "e_darts_scheduler_base.h"
#include "threadedProcedure.h"
#include "codelet.h"

extern codelet_t _dartsFinalCodelet;
extern scheduler_t dartsRtScheduler;

/*
 * @brief runtime definition function
 *
 * function to define the runtime, its CAM, and the scheduling policies to
 * be used
 *
 */
void e_darts_rt(e_darts_cam_t userCAM, cu_scheduler_selector cu_scheduler_policy, su_scheduler_selector su_scheduler_policy);

/*
 * @brief runtime reset initialization
 *
 * this function makes the runtime go back to the initial state. This is, clear all memories, clear all queues and
 * all other variables. This will allow the user to call another time e_darts_run()
 */

void e_darts_reset();

/*
 * @brief call final codelet
 *
 * This function pushes the final codelet to a CU so that the runtime can
 * finishes
 */
void e_darts_finilize();

#define DEC_DEP(syncSlot) dartsRtScheduler.decDep(syncSlot)
#define ADD_CODELET(codelet) dartsRtScheduler.add(codelet)
#define INVOKE(TPname, ...) {\
            TPname##_tpClosure_t newTpClosure = _invoke_##TPname(__VA_ARGS__);\
            dartsRtScheduler.invokeTP((genericTpClosure_t*)(&newTpClosure));\
        }

