/**
 * @file e_darts_scheduler_base.h
 * @author Jose M Monsalve
 * @date 9 August 2017
 * @brief darts Runtime Functions Pointers
 * @todo Add copyright
 *
 * This file defines the structure for the scheduler functions that are used
 * by the CUs, the SUs, and the user through e_darts_rt.h
 */

#include "syncSlot.h"
#include "codelet.h"
#include "tpClosure.h"

typedef struct __attribute__ ((__packed__)) scheduler_s {
    void (*policy)();
    void (*decDep)(syncSlot_t * toDecDep);
    void (*addCodelet)(codelet_t * toAdd);
    void (*invokeTP)(genericTpClosure_t * tpClosure);
} scheduler_t;

extern void * dartsUnitElement;
