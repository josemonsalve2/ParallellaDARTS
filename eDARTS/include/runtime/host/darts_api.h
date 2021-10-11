#include <e-hal.h>
#include <stdio.h>
#include <stdbool.h>
#include "darts_rt_params.h"

#define E_DARTS_OK 0
#define DARTS_RT_ALIVE 0x6c
#define MAX_PAYLOAD_SIZE 0x7f //127 for alignment
#define COMM_QUEUE_ADDRESS 0x8e000130 //based on print statement
#define COMM_QUEUE_OFFSET 0x00000130
#define BASE_OFFSET 0x00000000
#define SU_TO_NM_OFFSET 0x00000000
#define HEAD_OFFSET 0x00000000
#define TAIL_OFFSET 0x00000004
#define FULL_FLAG_OFFSET 0x00000008
#define NM_OP_OFFSET 0x0000000c
#define QUEUE_START_OFFSET 0x00000010
#define INDEX_OFFSET(index) (QUEUE_START_OFFSET + ((unsigned)index) * (17 + MAX_PAYLOAD_SIZE))
#define HEADER_OFFSET 0x00000000
#define PAYLOAD_OFFSET 0x00000008
#define ACK_OFFSET (PAYLOAD_OFFSET + MAX_PAYLOAD_SIZE)
#define SIGNAL_OFFSET (ACK_OFFSET + 0x00000001)
#define NM_TO_SU_OFFSET (16 + _DARTS_COMM_QUEUE_LENGTH * (13 + MAX_PAYLOAD_SIZE))

// doxygen?

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
} messageType;

// 4 + 4 = 8 bytes of header
typedef struct __attribute__ ((__packed__)) header_s {
    messageType msg_type;
    unsigned size;
} header_t; //on receive allocate memory for struct + size, load payload into void pointer

// 8 + MAX_PAYLOAD_SIZE + 1 + 4  = 13 + MAX_PAYLOAD_SIZE per mailbox
// in future may be better to remove ack and move signal before data, so that only valid data can be sent
// without requiring two e_writes
typedef struct __attribute__ ((__packed__)) mailbox_s {
    header_t msg_header;
    char data[MAX_PAYLOAD_SIZE];
    bool ack;
    message signal;
} mailbox_t;

// 4 + 4 + 4 + 4 + _DARTS_COMM_QUEUE_LENGTH * (13 + MAX_PAYLOAD_SIZE)
typedef struct __attribute__ ((__packed__)) commQueue_s {
    unsigned head_index;
    unsigned tail_index;
    unsigned full_flag;
    unsigned NM_op_done;
    //unsigned lock; //to mirror structure on Epiphany side
    mailbox_t queue[_DARTS_COMM_QUEUE_LENGTH];
} commQueue_t;

typedef struct __attribute__ ((__packed__)) commSpace_s {
    commQueue_t SUtoNM;
    commQueue_t NMtoSU;
} commSpace_t;

// 34 + 2 * MAX_PAYLOAD_SIZE overall <- this is outdated
typedef struct __attribute__ ((__packed__)) nodeMailbox_s {
    mailbox_t SUtoNM;
    mailbox_t NMtoSU;
} nodeMailbox_t;

typedef struct __attribute__ ((__packed__)) sigWithAck_s {
    bool ack;
    message signal;
} sigWithAck_t;

typedef union int_data_u {
    char raw[4];
    int processed;
} int_converter;

typedef union unsigned_data_u {
    char raw[4];
    unsigned processed;
} unsigned_converter;

e_platform_t platform;
e_epiphany_t dev;

extern nodeMailbox_t localMailbox;

int darts_init();

// non blocking; should change this later to allow the elf file to be independently selected
// add error checking
void darts_run(char* elf_file_name);

// wait for the runtime to close out
void darts_wait();

void darts_close();

int darts_send_message(message *signal);

int darts_send_message_wait(message *signal);

int darts_send_data(mailbox_t *data_loc);

int darts_send_data_wait(mailbox_t *data_loc);

// need to add generic tp closure to header definition and such
int darts_invoke_TP(void* closure);

message darts_receive_message(message *signal);

message darts_receive_data(mailbox_t* mailbox);

//helper function to fill mailbox data easier
void darts_fill_mailbox(mailbox_t *mailbox, messageType type, unsigned size, message signal);

int darts_set_ack(bool ack);

bool darts_get_ack();

int darts_data_convert_to_int(char *data);

unsigned darts_data_convert_to_unsigned(char *data);

void darts_int_convert_to_data(int input, char *data);

void darts_unsigned_convert_to_data(unsigned input, char *data);

//array of counts of args in following order: int, unsigned, char, float
unsigned short darts_args_encoding(unsigned short *type_array);

void darts_args_decoding(unsigned short code, unsigned short *type_array);
