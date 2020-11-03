#include "e_darts_cu.h"

// DO NOT USE THIS SYMBOL, IT IS USED TO GET THE SIZE IN LINKER SCRIPT
cu_runtime_elements_t _dartsCUElements __attribute__ ((section(".dartsCUElements")));
extern scheduler_t dartsRtScheduler;

void _CU_rt(cu_scheduler_selector cu_scheduler_policy) {
     //SU hardcoded below
     _dartsCUElements.mySUElements = (su_runtime_elements_t *) DARTS_APPEND_COREID(0x808,&(_dartsCUElements));
     unsigned thisCoreID;
     DARTS_GETCOREID(thisCoreID);
     codeletsQueue_t *codelet_queue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsCUElements.darts_rt_codeletsQueue));
     initCodeletsQueue(codelet_queue, _DARTS_CODELET_QUEUE_SIZE_BYTES);
     scheduler_t *cu_scheduler = (scheduler_t *) DARTS_APPEND_COREID(thisCoreID,&dartsRtScheduler);
     darts_set_cu_scheduler(cu_scheduler, cu_scheduler_policy);
}
