#include "threadedProcedure.h"

_tp_metadata_t _genericMetadataCtro(unsigned _TPid,
                    unsigned numSyncSlots, unsigned sizeDRAM,
                    unsigned sizeLocal, unsigned sizeDist) {
    _tp_metadata_t newMetadata;
    newMetadata._TPid = _TPid;
    newMetadata.numSyncSlots = numSyncSlots;
    newMetadata.refCount = 0;
    newMetadata.sizeDRAM = sizeDRAM;
    newMetadata.sizeLocal = sizeLocal;
    newMetadata.sizeDist = sizeDist;
    newMetadata.lock = DARTS_MUTEX_NULL;

    // Initialize syncSlots to zero
    int i;
    syncSlot_t* theSyncSlot = GET_SYNC_SLOT(newMetadata,0);
    for ( i = 0; i < numSyncSlots; i++)
    {
        codelet_t emptyCodelet;
        initCodelet(&emptyCodelet,0,theSyncSlot,__emptyCodeletFunction);
        initSyncSlot(theSyncSlot,i,0,0,emptyCodelet,0);
        theSyncSlot++;
    }


    return newMetadata;
}
