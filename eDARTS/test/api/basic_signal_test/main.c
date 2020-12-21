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

    usleep(1000);
    //send an arbitrary signal to the SU
    message signal = NM_REQUEST_SU_PROVIDE;
    //while (darts_send_message(&signal) == -1);
    printf("ack address from host: %x\n", MAILBOX_ADDRESS + NM_TO_SU_OFFSET + ACK_OFFSET);
    darts_send_message_wait(&signal);
    //if (darts_send_message(&signal) == -1) {
    //    printf("signal send failed; ack not set\n");
    //} 
    //printf("NM_REQUEST_SU_PROVIDE signal sent\n");
    //wait to receive an arbitrary signal in response
    message received;
    darts_receive_message(&received);
    while(received != SU_MAILBOX_ACCEPT) {
        printf("host received message: %d\n", received); 
        unsigned x = 0;
        while (x<100000000) x++;	
	darts_receive_message(&received);
    }
    printf("SU_MAILBOX_ACCEPT received by host\n");

    //wait for runtime to shut itself down
    darts_wait();
    //close the platform
    darts_close();
    return EXIT_SUCCESS;
}
