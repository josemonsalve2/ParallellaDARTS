#include "syncSlot.h"
#include "e_darts_print.h"

void initSyncSlot (syncSlot_t * newSyncSlot, unsigned newSlotID,
                   unsigned resetDep, unsigned initDep,
                   codelet_t fireCodeletTemplate, unsigned numCodeletsToFire)
{
    newSyncSlot->slotID = newSlotID;
    newSyncSlot->resetDep = resetDep;
    newSyncSlot->currentDep = initDep;
    newSyncSlot->codeletTemplate = fireCodeletTemplate;
    // We need the full address to lock this mutex
    newSyncSlot->lockMutex = DARTS_MUTEX_NULL;
    // We obtain the mutex address
    // TODO: MUTEX ADDRESS CAN BE OBTAINED FROM newSyncSlot or the incomming syncSlot.
    // This field is unnecessary.
    unsigned __coreid;
	__asm__ __volatile__ ("movfs %0, coreid" : "=r" (__coreid));
	newSyncSlot->lockMutexFullAddressPtr = (darts_mutex_t *) ((__coreid << 20) +
	                                       (unsigned)(((unsigned)&(newSyncSlot->lockMutex)) & 0x000FFFFF));
	newSyncSlot->numCodelets = numCodeletsToFire;
}


int syncSlotDecDep( syncSlot_t * syncSlot )
{
    //e_darts_print("syncSlot address: %x\n", syncSlot);
    //e_darts_print("trying to acquire lock at full address %x\n", syncSlot->lockMutexFullAddressPtr);
    //e_darts_print("full address dereferenced: %x\n", *(syncSlot->lockMutexFullAddressPtr));
    //e_darts_print("mutex variable set to %x\n", syncSlot->lockMutex);
    //e_darts_print("mutex variable address %x\n", &(syncSlot->lockMutex));
    // decrementing dependency.
    darts_mutex_lock(syncSlot->lockMutexFullAddressPtr);
    syncSlot->currentDep--;
    //e_darts_print("decreased dependencies to %d\n", syncSlot->currentDep);
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

