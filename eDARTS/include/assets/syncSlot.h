/**
 * @file syncSlot.h
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

#ifndef SYNCSLOT_H
#define SYNCSLOT_H

#include <stdio.h>
#include <stdlib.h>
#include "codelet.h"
#include "e_darts_mutex.h"
#include "threadedProcedure.h"

/**
 * @brief syncronization slots struct
 *
 * This struct contains the required variables and information
 * for synchronization between codelets. Each synchronization
 * represent a data or control dependency.
 *
 * SyncSlot size = 4 + 4 + 4 + 4 + 12 + 4 + 4 + 4 = 40 = 0x28
 *
 */
typedef struct __attribute__ ((__packed__)) syncSlot_s {
    unsigned slotID;
    unsigned resetDep;
    unsigned currentDep;
    _tp_metadata_t * tpFrame;
    codelet_t codeletTemplate;
    unsigned numCodelets;
    darts_mutex_t lockMutex;
    darts_mutex_t * lockMutexFullAddressPtr;
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

void initSyncSlot (syncSlot_t * newSyncSlot,unsigned slotID, unsigned resetDep, unsigned initDep, codelet_t fireCodeletTemplate, unsigned numCodeletsToFire);

/**
 * @brief This function allows decrementing a dependency of a syncronization slot
 * @todo. Find a better way to get the syncSlot's address or to express this requirement
 *
 * @p syncSlot synchronization slot to decrement dependency to
 *
 * @return Returns current dependency value
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


#endif /* SYNCSLOT_H */
