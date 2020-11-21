#include <e-hal.h>
#include <stdio.h>

#define MAX_PAYLOAD_SIZE 200 //check value
#define MAILBOX_ADDRESS 0x8e000138 //based on print statement
#define SU_TO_NM_OFFSET 0
#define NM_TO_SU_OFFSET 0 //figure out later
#define HEADER_OFFSET 0


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
    //anything else?
} messageType;

typedef struct __attribute__ ((packed)) header_s {
    messageType msg_type;
    unsigned size;
    void *msg;
} header_t; //on receive allocate memory for struct + size, load payload into void pointer

typedef struct __attribute__ ((packed)) mailbox_s {
    header_t msg_header;
    char data[DARTS_MAILBOX_MSG_SIZE]; //this is not defined yet
    message signal;
    unsigned lock; //no e_darts_mutex in arm so use this to fill the space
} mailbox_t;

typedef struct __attribute__ ((packed)) nodeMailbox_s {
    mailbox_t SUtoNM;
    mailbox_t NMtoSU;
} nodeMailbox_t;

int start_data_server();

void* printing_server(void *tid, void *output_location);

void stop_printing_server();
