#include "e_darts_su.h"

// DO NOT USE THIS SYMBOL, IT IS USED TO GET THE SIZE IN LINKER SCRIPT
su_runtime_elements_t _dartsSUElements __attribute__ ((section(".dartsSUElements")));
tp_heap_space_t _dartsTpHeap __attribute__ ((section(".dartsTpHeap")));

void _SU_rt(su_scheduler_selector su_scheduler_policy) {
     unsigned thisCoreID;
     DARTS_GETCOREID(thisCoreID);
     su_runtime_elements_t *mySUElements = (su_runtime_elements_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements));
     //only 15 because 0,0 is the SU
     mySUElements->myCUElements[1] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x809,&(_dartsSUElements));
     mySUElements->myCUElements[2] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x80a,&(_dartsSUElements));
     mySUElements->myCUElements[3] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x00b,&(_dartsSUElements));
     mySUElements->myCUElements[4] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x848,&(_dartsSUElements));
     mySUElements->myCUElements[5] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x849,&(_dartsSUElements));
     mySUElements->myCUElements[6] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x84a,&(_dartsSUElements));
     mySUElements->myCUElements[7] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x84b,&(_dartsSUElements));
     mySUElements->myCUElements[8] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x888,&(_dartsSUElements));
     mySUElements->myCUElements[9] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x889,&(_dartsSUElements));
     mySUElements->myCUElements[10] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x88a,&(_dartsSUElements));
     mySUElements->myCUElements[11] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x88b,&(_dartsSUElements));
     mySUElements->myCUElements[12] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x8c8,&(_dartsSUElements));
     mySUElements->myCUElements[13] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x8c9,&(_dartsSUElements));
     mySUElements->myCUElements[14] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x8ca,&(_dartsSUElements));
     mySUElements->myCUElements[15] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x8cb,&(_dartsSUElements));
     //SU position is hardcoded in below and above
     scheduler_t *su_scheduler = (scheduler_t *) DARTS_APPEND_COREID(thisCoreID,&(dartsRtScheduler));
     darts_set_su_scheduler(su_scheduler, su_scheduler_policy);
}
