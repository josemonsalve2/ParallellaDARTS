#include "e_darts_su_scheduler.h"

su_scheduler_t _darts_su_scheduler;

// Policies
void su_scheduler_round_robin();

void darts_set_su_scheduler(su_scheduler_selector su_scheduler_policy) {
    switch(su_scheduler_policy) {
    case SU_ROUND_ROBIN:
        _darts_su_scheduler.policy = su_scheduler_round_robin();
        break;
    default:
        break;
    }
}
