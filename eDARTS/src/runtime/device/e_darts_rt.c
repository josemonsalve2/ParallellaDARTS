#include "e_darts_rt.h"

void terminationCodelet(){
    // Iterate over all the cores and do this
    darts_rt_alive = 0;
}

codelet_t _dartsFinalCodelet __attribute__ ((section(".dartsFinalCodelet"))) = {0xFFFFFFFF, 0, terminationCodelet};

void e_darts_rt(e_darts_cam_t userCAM, cu_scheduler_selector cu_scheduler_policy, su_scheduler_selector su_scheduler_policy) {
    // Initialize the runtime here
    // copy all the structures to other cores
    // call initialization for each of the other core's structures

    // Iterate over all the cores and do this
    darts_rt_alive = 1;
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0)
    {
        _SU_rt(su_scheduler_policy);
    }
    else
    {
        _CU_rt(cu_scheduler_policy);
    }
}

void e_darts_reset() {
    // Reinitialize runtime here

    // iterate over all the cores and do this
    darts_rt_alive = 1;
}

void e_darts_run() {
    // push to the queue and get stuck in the policy
    dartsRtScheduler.policy();
}

void e_darts_finilize() {

}

