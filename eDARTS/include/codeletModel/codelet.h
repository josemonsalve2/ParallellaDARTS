/**
 * @file codelet.h
 * @author Jose M Monsalve
 * @date 26 May 2017
 * @brief Definition of the synchronization slots
 * @todo Add copyright
 *
 * Codelets are the unito of computation of the codelet model.
 * A codelet represents a task and it is a non-preemtible segment
 * of code.
 * A codelet can be waiting for dependencies, enabled, ready or fired.
 * Enabled codelets have all their dependencies satisfied, but have not been
 * allocated to a CU nor the code prefetched. ready codelets are allocated
 * but have not been scheduled for execution, while fired codelets are codelets
 * that are executing or have been already executed
 * A codelet has an ID that can be used as reference within the computation
 * A codelet always belongs to a syncronization slot, which is where
 * dependencies are kept. A syncronization slot might fire multiple
 * codelets.
 * A codelet can use the syncslot to obtian the TP frame
 */

#ifndef CODELET_H
#define CODELET_H

#include <stdio.h>
#include <stdlib.h>

// A codelet is just a function pointer
typedef void (*codeletFunction)();

// Forward declaration
typedef struct __attribute__ ((__packed__)) syncSlot_s syncSlot_t;

/**
 * @brief codeletStruct
 *
 * This struct contains the required variables and information
 * for the codelets. Each codelet is a unit of computation and
 * is associated with a synchronization slot. Each codelet
 * has an ID within that synchronization slot and can be used within
 * the codelet
 *
 * codelet_t size = 4 + 4 + 4 = 12 = 0xC
 */
typedef struct __attribute__ ((__packed__)) codelet_s {
    unsigned codeletID;
    syncSlot_t * syncSlot;
    codeletFunction fire;
} codelet_t;

/**
 * @brief initCodelet
 *
 * codelet creation method.
 *
 */
void initCodelet(codelet_t * newCodelet, unsigned newID, syncSlot_t * newSyncSlot, codeletFunction newFire);

/**
 * @brief function that does nothing for empty codelets
 *
 */

void __emptyCodeletFunction();

#endif /* CODELET_H */
