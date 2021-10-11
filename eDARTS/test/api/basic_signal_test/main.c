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
    //message signal = NM_REQUEST_SU_PROVIDE;
    printf("ack address from host: %x\n", MAILBOX_ADDRESS + NM_TO_SU_OFFSET + ACK_OFFSET);
    //darts_send_message_wait(&signal); // start with blocking, wait for mailbox to be initialized
    mailbox_t suMailbox;
    suMailbox.msg_header.msg_type = DATA;
    suMailbox.msg_header.size = (unsigned) sizeof(unsigned) + sizeof(int);
    darts_fill_mailbox(&suMailbox, DATA, sizeof(unsigned)+sizeof(int), NM_REQUEST_SU_PROVIDE);
    darts_unsigned_convert_to_data(5U, suMailbox.data);
    darts_int_convert_to_data(-5, &(suMailbox.data[4]));
    printf("NM sending raw data: %x%x%x%x %x %x %x %x\n", suMailbox.data[0], suMailbox.data[1], suMailbox.data[2], suMailbox.data[3], suMailbox.data[4], suMailbox.data[5], suMailbox.data[6], suMailbox.data[7]);

    darts_send_data_wait(&suMailbox);
    //wait to receive an arbitrary signal in response
    mailbox_t nmMailbox;
    while(darts_get_ack());
    darts_receive_data(&nmMailbox);
    int int_data = darts_data_convert_to_int(nmMailbox.data);
    unsigned uns_data = darts_data_convert_to_unsigned(&(nmMailbox.data[4]));
    printf("NM received raw data: %x%x%x%x %x %x %x %x\n", nmMailbox.data[0], nmMailbox.data[1], nmMailbox.data[2], nmMailbox.data[3], nmMailbox.data[4], nmMailbox.data[5], nmMailbox.data[6], nmMailbox.data[7]);
    printf("NM received signal: %d\n", nmMailbox.signal);
    printf("NM received int value %d and uns value %u\n", int_data, uns_data); 

    //wait for runtime to shut itself down
    darts_wait();
    //close the platform
    darts_close();
    return EXIT_SUCCESS;
}
