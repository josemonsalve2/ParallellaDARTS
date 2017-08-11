#include "e_darts_cu_scheduler.h"

// Scheduler Policies
void cu_scheduler_round_robin() {

}

// decDep Policies
void decDepAndPush(){

}

// addCodelet Policies
void addCodeletSelfQueue() {

}

// invoke Policies
void invokeSelfQueue() {

}

void darts_set_cu_scheduler(scheduler_t* cu_scheduler, cu_scheduler_selector cu_scheduler_policy) {
    switch(cu_scheduler_policy) {
    case CU_ROUND_ROBIN:
        cu_scheduler->policy = cu_scheduler_round_robin();
        break;
    default:
        break;
    }
}
