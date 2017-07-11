#include "syncSlot.h"

void initSyncSlot (syncSlot_t * newSyncSlot, unsigned newSlotID, unsigned resetDep, unsigned initDep, codelet_t fireCodeletTemplate, unsigned numCodeletsToFire)
{
    newSyncSlot->slotID = newSlotID;
    newSyncSlot->resetDep = resetDep;
    newSyncSlot->currentDep = initDep;
    newSyncSlot->codeletTemplate = fireCodeletTemplate;
    // We need the full address to lock this mutex
    newSyncSlot->lockMutex = DARTS_MUTEX_NULL;
    COMPLETE_ADDRESS(newSyncSlot->lockMutex, *(newSyncSlot->lockMutexFullAddressPtr));
}


int syncSlotDecDep( syncSlot_t * syncSlot )
{
    // decrementing dependency.
    darts_mutex_lock(syncSlot->lockMutexFullAddressPtr);
    syncSlot->currentDep--;
    darts_mutex_unlock(syncSlot->lockMutexFullAddressPtr);
    return syncSlot->currentDep;
}

void syncSlotResetDep( syncSlot_t * syncSlot )
{
    // reseting the dependencies
    darts_mutex_lock(syncSlot->lockMutexFullAddressPtr);
    syncSlot->currentDep = syncSlot->resetDep;
    darts_mutex_unlock(syncSlot->lockMutexFullAddressPtr);
}

