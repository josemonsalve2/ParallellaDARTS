/**
 * @file e_darts_su_scheduler.h
 * @author Jose M Monsalve
 * @date 7 August 2017
 * @brief SU scheduler definitions
 * @todo Add copyright
 * @todo add printing of variables
 *
 * Definition of the SU scheduler function
 *
 */
#ifndef _E_DARTS_SU_SCHEDULER_H_
#define _E_DARTS_SU_SCHEDULER_H_

#include "syncSlot.h"
#include "codelet.h"
#include "threadedProcedure.h"
#include "e_darts_scheduler_base.h"
#include "e_darts_global.h"
#include "e_darts_mailbox.h"
#include "e_darts_su.h"

typedef enum {
    SU_ROUND_ROBIN
} su_scheduler_selector;

void su_scheduler_round_robin();
void pushFinalCodelets(codeletsQueue_t *cuCodeletQueue, codelet_t *toFire);
void deployCodelet(codeletsQueue_t *cuCodeletQueue, unsigned cuIndex, codelet_t *toFire);
void su_decDepAndPush(syncSlot_t * toDecDep);
void su_addCodeletSelfQueue(codelet_t * toAdd);
void su_invokeSelfQueue(genericTpClosure_t * tpClosure);
void darts_set_su_scheduler(scheduler_t* su_scheduler, su_scheduler_selector su_scheduler_policy);

void suMailboxCheck(mailbox_t *suMailbox, mailbox_t *nmMailbox);

#endif /* _E_DARTS_SU_SCHEDULER_H_ */
