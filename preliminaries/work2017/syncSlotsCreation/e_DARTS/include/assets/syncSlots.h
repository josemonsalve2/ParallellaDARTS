/**
 * @file syncSlots.h
 * @author Jose M Monsalve
 * @date 26 May 2017
 * @brief Definition of the synchronization slots
 * @todo Add copyright
 *
 * Syncronization slots are the part of the runtime that 
 * receives the signals from previous events or codelets. They 
 * express when the required data, control or resource
 * dependecies have been already satisfied. 
 */

#include <stdio.h>
#include <stdlib.h>
#include "common.h"

/**
 * @brief syncronization slots struct
 *
 * This struct contains the required variables and information
 * for synchronization between codelets. Each synchronization 
 * represent a data or control dependency.
 *
 * @todo Add multiple fire codelets to this syncslot
 *
 */
typedef struct PACKED syncSlot_s {
    unsigned resetDep;
    unsigned currentDep;
    codelet fire;
    darts_mutex_t lockMutex;
} syncSlot_t;

/**
 * @brief This function allows to initialize a new synchronization slot. 
 *
 * @p newSyncSlot Pointer to the new synchronization slot to initialize
 * @p resetDep Reset dependencies. Howe many initial dependencies exist
 * @p initDep Initial value for the current dependency counter
 * @p fireCodelet This is the codelet to run whenever the dependencies
 * are satisfied. 
 *
 */

void initSyncSlot ( syncSlot_t * newSyncSlot, unsigned resetDep, unsigned initDep, codelet fireCodelet);

/**
 * @brief This function allows decrementing a dependency of a syncronization slot 
 * @todo. Find a better way to get the syncSlot's address or to express this requirement
 *
 * @p syncSlot synchronization slot to decrement dependency to
 *
 * @return Returns 1 if dependencies are satisfied (currentDep == 0)
 */
int syncSlotDecDep( syncSlot_t * syncSlot );

/**
 * @brief Reset the syncSlot counter
 * @todo. Find a better way to get the syncSlot's address or to express this requirement
 *
 * @p syncSlot synchronization slot to decrement dependency to
 *
 */
void syncSlotResetDep( syncSlot_t * syncSlot );
