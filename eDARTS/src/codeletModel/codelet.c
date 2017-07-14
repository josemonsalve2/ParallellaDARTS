#include "codelet.h"

void initCodelet(codelet_t * newCodelet, unsigned newID, syncSlot_t * newSyncSlot, codeletFunction newFire)
{
    newCodelet->codeletID = newID;
    newCodelet->syncSlot = newSyncSlot;
    newCodelet->fire = newFire;
}

void __emptyCodeletFunction(){}
