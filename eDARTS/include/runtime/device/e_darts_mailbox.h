#ifndef E_DARTS_MAILBOX_H
#define E_DARTS_MAILBOX_H

#include "tpClosure.h"
#include "e_darts_mutex.h"


typedef enum message_select {
    blank = 0,
    SU_MAILBOX_REJECT = 1,      //SU rejects incoming data
    SU_MAILBOX_ACCEPT = 2,      //SU accepts incoming data
    NM_MAILBOX_REJECT = 3,      //NM rejects incoming data
    NM_MAILBOX_ACCEPT = 4,      //NM accepts incoming data
    NM_REQUEST_SU_RECEIVE = 5,  //NM requests SU receive some data, ex: invoke new closure from NM
    NM_REQUEST_SU_PROVIDE = 6,  //NM requests SU provide some data
    SU_REQUEST_NM_RECEIVE = 7,  //SU requests NM receive some data
    SU_REQUEST_NM_PROVIDE = 8   //SU requests NM provide some data
    //reset?
} message;

typedef enum message_type {
    DATA = 1,
    TPCLOSURE = 2
    //anything else?
} messageType;

//move define statements for messages into an enum
//messages: invokeTP, reset runtime

typedef struct header_s {
    messageType msg_type;
    unsigned size;
    void *msg;
} header_t; //on receive allocate memory for struct + size, load payload into void pointer

typedef struct mailbox_s {
    header_t msg_header;
    char data[DARTS_MAILBOX_MSG_SIZE]; //this is not defined yet
    message signal;
    darts_mutex_t lock;
} mailbox_t;

typedef struct nodeMailbox_s {
    mailbox_t SUtoNM;
    mailbox_t NMtoSU;
} nodeMailbox_t;

extern nodeMailbox_t _dartsLocalMailbox;


