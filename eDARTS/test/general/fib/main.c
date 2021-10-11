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
    darts_run("e_darts_fib_test.elf");

    usleep(1000);
    /*
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
    mailbox_t nmMailbox;
    printf("host: waiting for receive\n");
    //while(!darts_get_ack());
    while(darts_get_ack());
    darts_receive_data(&nmMailbox);
    int int_data = darts_data_convert_to_int(nmMailbox.data);
    printf("NM received raw data: %x%x%x%x\n", nmMailbox.data[0], nmMailbox.data[1], nmMailbox.data[2], nmMailbox.data[3]);
    printf("NM received signal: %d\n", nmMailbox.signal);
    printf("NM received int value: %d\n", int_data); 
//    while (nmMailbox
    //need to add protection to receive: if receive already has ack set, return error, no new info
    //add get_ack function and do before receive, add new function receive_new_data that returns error
    //if ack is already true. How does the memory distance for the api side affect this protocol?
    //Is this an insufficient process because of memory latency?
    
    //while( != SU_MAILBOX_ACCEPT) {
   //     printf("host received message: %d\n", received); 
  //      unsigned x = 0;
 //       while (x<100000000) x++;	
//	darts_receive_message(&received);
    //}
    
    //printf("SU_MAILBOX_ACCEPT received by host\n");
    */
    //wait for runtime to shut itself down
    darts_wait();
    //close the platform
    darts_close();
    return EXIT_SUCCESS;
}
