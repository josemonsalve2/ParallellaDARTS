#ifndef E_DARTS_MAILBOX_H
#define E_DARTS_MAILBOX_H

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

// 8 bytes of header
typedef struct __attribute__ ((__packed__)) header_s {
    messageType msg_type;
    unsigned size;
} header_t; //on receive allocate memory for struct + size, load payload into void pointer

// 12 + _DARTS_MAILBOX_MSG_SIZE + 1 + 4 = 17 + _DARTS_MAILBOX_MSG_SIZE per mailbox
// note: for proper use _DARTS_MAILBOX_MSG_SIZE has to be the same as MAX_PAYLOAD_SIZE
//       on the host side
typedef struct __attribute__ ((__packed__)) mailbox_s {
    header_t msg_header;
    char data[_DARTS_MAILBOX_MSG_SIZE]; //this is not defined yet
    bool ack;
    message signal;
} mailbox_t;

typedef struct __attribute__ ((__packed__)) commQueue_s {
    unsigned head_index;
    unsigned tail_index;
    unsigned full_flag;
    unsigned NM_op_done;
    //e_darts_mutex lock;
    mailbox_t queue[_DARTS_COMM_QUEUE_LENGTH];
} commQueue_t;

typedef struct __attribute__ ((__packed__)) commSpace_s {
    commQueue_t SUtoNM;
    commQueue_t NMtoSU;
} commSpace_t;

/*
// 34 + 2 * MAX_PAYLOAD_SIZE overall
typedef struct __attribute__ ((__packed__)) nodeMailbox_s {
    mailbox_t SUtoNM;
    mailbox_t NMtoSU;
} nodeMailbox_t;
*/

typedef union int_data_u {
    char raw[4];
    int processed;
} int_converter;

typedef union unsigned_data_u {
    char raw[4];
    unsigned processed;
} unsigned_converter;

//extern nodeMailbox_t _dartsNodeMailbox;

void e_darts_print_SU_queue();
void e_darts_print_NM_queue();
void e_darts_comm_init();

message e_darts_receive_signal();

message e_darts_receive_data(mailbox_t *loc);

int e_darts_send_signal(message *signal);

void e_darts_fill_mailbox(mailbox_t *mailbox, messageType type, unsigned size, message signal);

int e_darts_send_data(mailbox_t *loc);

int e_darts_get_queue_space(commQueue_t *queue);

void e_darts_set_ack(bool ack);

bool e_darts_get_ack();

void e_darts_int_convert_to_data(int input, char *data);

void e_darts_unsigned_convert_to_data(unsigned input, char *data);

int e_darts_data_convert_to_int(char *data);

unsigned e_darts_data_convert_to_unsigned(char *data);

unsigned short e_darts_args_encoding(unsigned short *type_array);

void e_darts_args_decoding(unsigned short code, unsigned short *type_array);

#endif /* E_DARTS_MAILBOX_H */
