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

    return newMetadata;
}
