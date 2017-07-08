#include "syncSlot.h"

void initSyncSlot (syncSlot_t * newSyncSlot, unsigned newSlotID, unsigned resetDep, unsigned initDep, codelet_t fireCodeletTemplate, unsigned numCodeletsToFire)
{
    newSyncSlot->slotID = newSlotID;
    newSyncSlot->resetDep = resetDep;
    newSyncSlot->currentDep = initDep;
    newSyncSlot->codeletTemplate = fireCodeletTemplate; 
    newSyncSlot->lockMutex = DARTS_MUTEX_NULL;
}


int syncSlotDecDep( syncSlot_t * syncSlot )
{
    // mutex must have the coreID in the address
    unsigned mutexAddress;
    COMPLETE_ADDRESS(syncSlot->lockMutex, mutexAddress);
    
    // decrementing dependency.
    darts_mutex_lock((darts_mutex_t *) mutexAddress);
    syncSlot->currentDep--;
    darts_mutex_unlock((darts_mutex_t *) mutexAddress);
    return syncSlot->currentDep;
}

void syncSlotResetDep( syncSlot_t * syncSlot )
{
    // mutex must have the coreID in the address
    unsigned mutexAddress;
    COMPLETE_ADDRESS(syncSlot->lockMutex, mutexAddress);
    
    // reseting the dependencies
    darts_mutex_lock((darts_mutex_t *) mutexAddress);
    syncSlot->currentDep = syncSlot->resetDep;
    darts_mutex_unlock((darts_mutex_t *) mutexAddress);
}

