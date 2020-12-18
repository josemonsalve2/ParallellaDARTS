#ifndef E_DARTS_MAILBOX_H
#define E_DARTS_MAILBOX_H
#endif

#include <stdbool.h>
#include "tpClosure.h"
#include "e_darts_mutex.h"
#include "darts_rt_params.h"


typedef enum message_select {
    blank = 0,
    SU_MAILBOX_REJECT = 1,      //SU rejects incoming data
    SU_MAILBOX_ACCEPT = 2,      //SU accepts incoming data
    NM_MAILBOX_REJECT = 3,      //NM rejects incoming data
    NM_MAILBOX_ACCEPT = 4,      //NM accepts incoming data
    NM_REQUEST_SU_RECEIVE = 5,  //NM requests SU receive some data, ex: invoke new closure from NM
    NM_REQUEST_SU_PROVIDE = 6,  //NM requests SU provide some data
    SU_REQUEST_NM_RECEIVE = 7,  //SU requests NM receive some data
    SU_REQUEST_NM_PROVIDE = 8,   //SU requests NM provide some data
    NM_REQUEST_STATUS = 9,
    SU_PROVIDE_STATUS = 10
    //reset?
} message;

typedef enum message_type {
    DATA = 1,
    TPCLOSURE = 2,
    STATUS = 3
    //anything else?
} messageType;

//move define statements for messages into an enum
//messages: invokeTP, reset runtime

typedef struct __attribute__ ((__packed__)) header_s {
    messageType msg_type;
    unsigned size;
    void *msg;
} header_t; //on receive allocate memory for struct + size, load payload into void pointer

typedef struct __attribute__ ((__packed__)) mailbox_s {
    header_t msg_header;
    char data[_DARTS_MAILBOX_MSG_SIZE]; //this is not defined yet
    bool ack;
    message signal;
    darts_mutex_t lock;
} mailbox_t;

typedef struct __attribute__ ((__packed__)) nodeMailbox_s {
    mailbox_t SUtoNM;
    mailbox_t NMtoSU;
} nodeMailbox_t;

extern nodeMailbox_t _dartsNodeMailbox;

message e_darts_receive_signal();

message e_darts_receive_data(mailbox_t *loc);

int e_darts_send_signal(message *signal);

int e_darts_send_data(mailbox_t *loc);

void e_darts_set_ack(bool ack);

unsigned short e_darts_args_encoding(unsigned short *type_array);

void e_darts_args_decoding(unsigned short code, unsigned short *type_array);
