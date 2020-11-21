#include <e-hal.h>
#include <stdio.h>

#define E_DARTS_OK 0


e_platform_t platform;
e_epiphany_t dev;

typedef struct header_s {
    messageType msg_type;
    unsigned size;
    void *msg;
} header_t; //on receive allocate memory for struct + size, load payload into void pointer

typedef enum messages {
   // get messages from mailbox.h 
} msg;

int e_darts_init();

// non blocking; should change this later to allow the elf file to be independently selected
// add error checking
void e_darts_run();

// wait for the runtime to close out
void e_darts_wait();

void e_darts_close();

int e_darts_send_message(msg message);

// need to add generic tp closure to header definition and such
int e_darts_invoke_TP(tp_closure_t closure);
