#ifndef E_DARTS_MAILBOX_H
#define E_DARTS_MAILBOX_H

#include "tpClosure.h"
#include "e_darts_mutex.h"

//lets define some signals here
//note: SU or NM denotes whose action the signal refers to: SU_MAILBOX_REJECT meants it is rejecting the contents
//      of the mailbox that handles NM->SU communication
#define SU_MAILBOX_REJECT 1      //SU rejects work
#define SU_MAILBOX_ACCEPT 2      //SU accepts work
#define NM_MAILBOX_REJECT 3      //NM rejects work
#define NM_MAILBOX_ACCEPT 4      //NM accepts works
#define NM_REQUEST_SU_RECEIVE 5  //NM tells SU it wants to give work (requests the SU receive work)
#define NM_REQUEST_SU_PROVIDE 6  //NM tells SU it wants to receive work (requests the SU provide it work)
#define SU_REQUEST_NM_RECEIVE 7  //SU tells NM it wants to offload work (requests the NM receive some work)
#define SU_REQUEST_NM_PROVIDE 8  //SU tells NM it wants to accept more work (requests the NM provide it work)


//move define statements for messages into an enum
//messages: invokeTP, reset runtime
//do we need 4? I think we should be okay with 2: one for SU->NM and one for NM->SU

typedef struct header_s {
    //msgtype (can be enum?)
    //size
    //void *
} header_t; //on receive allocate memory for struct + size, load payload into void pointer

typedef struct mailbox_s {
    genericTpClosure_t closure; //not just a closure, space data w/ type
    unsigned signal;
    darts_mutex_t lock;
} mailbox_t;

typedef struct nodeMailbox_s {
    mailbox_t SUtoNM;
    mailbox_t NMtoSU;
} nodeMailbox_t;

extern nodeMailbox_t _dartsLocalMailbox;


