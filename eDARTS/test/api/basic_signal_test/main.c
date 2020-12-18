#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "e-hal.h"
#include "darts_print_server.h"
#include "darts_api.h"

#define FINAL_BARRIER 0x2008
#define DARTS_RT_ALIVE 0x6c


int main(int argc, char *argv[]){

    //initialize platform
    darts_init();
    //load elf file and execute on Epiphany
    darts_run("e_darts_mailbox_test.elf");

    //send an arbitrary signal to the SU
    message signal = NM_REQUEST_SU_PROVIDE;
    //int bytes;
    if (darts_send_message(&signal) == -1) {
        printf("signal send failed; ack not set\n");
    } 
    printf("NM_REQUEST_SU_PROVIDE signal sent\n");
    //wait to receive an arbitrary signal in response
    while(darts_receive_message(&signal) != SU_MAILBOX_ACCEPT);

    //wait for runtime to shut itself down
    darts_wait();
    //close the platform
    darts_close();
    return EXIT_SUCCESS;
}
