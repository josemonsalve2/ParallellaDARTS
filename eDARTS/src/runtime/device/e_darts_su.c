#include "e_darts_su.h"

// DO NOT USE THIS SYMBOL, IT IS USED TO GET THE SIZE IN LINKER SCRIPT
su_runtime_elements_t _dartsSUElements __attribute__ ((section(".dartsSUElements")));

void _SU_rt(su_scheduler_selector su_scheduler_policy) {
     unsigned thisCoreID;
     DARTS_GETCOREID(thisCoreID);
     su_runtime_elements_t *mySUElements = (su_runtime_elements_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements));
     //only 15 because 0,0 is the SU
     mySUElements->myCUElements[0] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x001,&(_dartsSUElements));
     mySUElements->myCUElements[1] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x002,&(_dartsSUElements));
     mySUElements->myCUElements[2] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x003,&(_dartsSUElements));
     mySUElements->myCUElements[3] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x040,&(_dartsSUElements));
     mySUElements->myCUElements[4] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x041,&(_dartsSUElements));
     mySUElements->myCUElements[5] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x042,&(_dartsSUElements));
     mySUElements->myCUElements[6] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x043,&(_dartsSUElements));
     mySUElements->myCUElements[7] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x080,&(_dartsSUElements));
     mySUElements->myCUElements[8] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x081,&(_dartsSUElements));
     mySUElements->myCUElements[9] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x082,&(_dartsSUElements));
     mySUElements->myCUElements[10] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x083,&(_dartsSUElements));
     mySUElements->myCUElements[11] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x0c0,&(_dartsSUElements));
     mySUElements->myCUElements[12] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x0c1,&(_dartsSUElements));
     mySUElements->myCUElements[13] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x0c2,&(_dartsSUElements));
     mySUElements->myCUElements[14] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x0c3,&(_dartsSUElements));
     //SU position is hardcoded in below and above
     scheduler_t *su_scheduler = (scheduler_t *) DARTS_APPEND_COREID(0x000,&(dartsRtScheduler));
     darts_set_su_scheduler(su_scheduler, su_scheduler_policy);
}
