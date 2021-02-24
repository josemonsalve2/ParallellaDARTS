#include "e_darts_su.h"
#include "e_darts_print.h"

// DO NOT USE THIS SYMBOL, IT IS USED TO GET THE SIZE IN LINKER SCRIPT
su_runtime_elements_t _dartsSUElements __attribute__ ((section(".dartsSUElements")));
tp_heap_space_t _dartsTpHeap __attribute__ ((section(".dartsTpHeap")));
tp_data_heap_space_t _tpDataHeap __attribute__ ((section(".tpDataHeap")));
tp_data_heap_DRAM_space_t _tpDataHeapDRAM __attribute__ ((section(".tpDataHeapDRAM")));
extern scheduler_t dartsRtScheduler;

void _SU_rt(su_scheduler_selector su_scheduler_policy) {
     unsigned thisCoreID;
     DARTS_GETCOREID(thisCoreID);
     //su_runtime_elements_t *mySUElements = (su_runtime_elements_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements));
     codeletsQueue_t *codelet_queue = (codeletsQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements.darts_rt_codeletsQueue));
     tpClosuresQueue_t *tp_queue = (tpClosuresQueue_t *) DARTS_APPEND_COREID(thisCoreID,&(_dartsSUElements.darts_rt_tpclosuresQueue));
     initCodeletsQueue(codelet_queue, _DARTS_CODELET_QUEUE_SIZE_BYTES);
     initTpClosuresQueue(tp_queue, _DARTS_TPCLOSURES_QUEUE_SIZE_BYTES);
     //only 15 because 0,0 is the SU
     _dartsSUElements.myCUElements[1] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x809,&(_dartsSUElements));
     _dartsSUElements.myCUElements[2] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x80a,&(_dartsSUElements));
     _dartsSUElements.myCUElements[3] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x80b,&(_dartsSUElements));
     _dartsSUElements.myCUElements[4] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x848,&(_dartsSUElements));
     _dartsSUElements.myCUElements[5] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x849,&(_dartsSUElements));
     _dartsSUElements.myCUElements[6] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x84a,&(_dartsSUElements));
     _dartsSUElements.myCUElements[7] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x84b,&(_dartsSUElements));
     _dartsSUElements.myCUElements[8] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x888,&(_dartsSUElements));
     _dartsSUElements.myCUElements[9] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x889,&(_dartsSUElements));
     _dartsSUElements.myCUElements[10] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x88a,&(_dartsSUElements));
     _dartsSUElements.myCUElements[11] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x88b,&(_dartsSUElements));
     _dartsSUElements.myCUElements[12] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x8c8,&(_dartsSUElements));
     _dartsSUElements.myCUElements[13] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x8c9,&(_dartsSUElements));
     _dartsSUElements.myCUElements[14] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x8ca,&(_dartsSUElements));
     _dartsSUElements.myCUElements[15] = (cu_runtime_elements_t *) DARTS_APPEND_COREID(0x8cb,&(_dartsSUElements));
     //SU position is hardcoded in below and above
     scheduler_t *su_scheduler = (scheduler_t *) DARTS_APPEND_COREID(thisCoreID,&(dartsRtScheduler));
     darts_set_su_scheduler(su_scheduler, su_scheduler_policy);
}
