#include <e-hal.h>
#include <stdio.h>
#include <stdbool.h>

#define E_DARTS_OK 0
#define DARTS_RT_ALIVE 0x6c
#define MAX_PAYLOAD_SIZE 0x7f //127 for alignment
#define MAILBOX_ADDRESS 0x8e000138 //based on print statement
#define BASE_OFFSET 0x0
#define SU_TO_NM_OFFSET 0x0
#define HEADER_OFFSET 0x0
#define PAYLOAD_OFFSET 0xc
#define ACK_OFFSET (PAYLOAD_OFFSET + MAX_PAYLOAD_SIZE)
#define SIGNAL_OFFSET (ACK_OFFSET + 0x1)
#define LOCK_OFFSET (SIGNAL_OFFSET + 0x4)
#define NM_TO_SU_OFFSET (LOCK_OFFSET + 0x4)

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
    TPCLOSURE = 2,
    STATUS = 3
} messageType;

// 4 + 4 + 4 = 12 bytes of header
typedef struct __attribute__ ((__packed__)) header_s {
    messageType msg_type;
    unsigned size;
    void *msg;
} header_t; //on receive allocate memory for struct + size, load payload into void pointer

// 12 + MAX_PAYLOAD_SIZE + 1 + 4 + 4 = 21 + MAX_PAYLOAD_SIZE per mailbox
typedef struct __attribute__ ((__packed__)) mailbox_s {
    header_t msg_header;
    char data[MAX_PAYLOAD_SIZE];
    bool ack;
    message signal;
    unsigned lock; //no e_darts_mutex in arm so use this to fill the space
} mailbox_t;

// 40 + 2 * MAX_PAYLOAD_SIZE overall
typedef struct __attribute__ ((__packed__)) nodeMailbox_s {
    mailbox_t SUtoNM;
    mailbox_t NMtoSU;
} nodeMailbox_t;

typedef struct __attribute__ ((__packed__)) sigWithAck_s {
    bool ack;
    message signal;
} sigWithAck_t;

e_platform_t platform;
e_epiphany_t dev;

extern nodeMailbox_t localMailbox;

int darts_init();

// non blocking; should change this later to allow the elf file to be independently selected
// add error checking
void darts_run();

// wait for the runtime to close out
void darts_wait();

void darts_close();

int darts_send_message(message *signal);

int darts_send_data(mailbox_t* data_loc);

// need to add generic tp closure to header definition and such
int darts_invoke_TP(void* closure);

message darts_receive_message(message *signal);

message darts_receive_data(mailbox_t* mailbox);

int darts_set_ack(bool ack);

//array of counts of args in following order: int, unsigned, char, float
unsigned short darts_args_encoding(unsigned short *type_array);

void darts_args_decoding(unsigned short code, unsigned short *type_array);
