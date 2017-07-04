#include "syncSlots.h"

void initSyncSlot ( syncSlot_t * newSyncSlot, unsigned resetDep, unsigned initDep, codelet fireCodelet)
{
    newSyncSlot->resetDep = resetDep;
    newSyncSlot->currentDep = initDep;
    newSyncSlot->fire = fireCodelet; 
    newSyncSlot->lockMutex = DARTS_MUTEX_NULL;
}


int syncSlotDecDep( syncSlot_t * syncSlot )
{
    darts_mutex_lock(syncSlot->lockMutex, (unsigned)(syncSlot & 0xfff00000);
    syncSlot->currentDep--;
    darts_mutex_unlock(syncSlot->lockMutex, (unsigned)(syncSlot & 0xfff00000);
    return (syncSlot->currentDep == 0);
}

void syncSlotResetDep( syncSlot_t * syncSlot )
{
    darts_mutex_lock(syncSlot->lockMutex, (unsigned)(syncSlot & 0xfff00000);
    syncSlot->currentDep = syncSlot->resetDep;
    darts_mutex_unlock(syncSlot->lockMutex, (unsigned)(syncSlot & 0xfff00000);
}

