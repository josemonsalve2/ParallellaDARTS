#include "e_darts_cu.h"

// DO NOT USE THIS SYMBOL, IT IS USED TO GET THE SIZE IN LINKER SCRIPT
cu_runtime_elements_t _dartsCUElements __attribute__ ((section(".dartsCUElements")));

void _CU_rt(cu_scheduler_selector cu_scheduler_policy) {
     mySUElements = (su_runtime_elements_t *) DARTS_APPEND_COREID(0x000,&(_dartsCUElements));
     unsigned thisCoreID;
     DARTS_GET_COREID(thisCoreID);
     scheduler_t *cu_scheduler = (scheduler_t *) DARTS_APPEND_COREID(thisCoreID,&dartsRtScheduler);
     darts_set_cu_scheduler(cu_scheduler, cu_scheduler_policy);
}
