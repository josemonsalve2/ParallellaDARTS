#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "e-hal.h"
#include "darts_print_server.h"
#include "darts_api.h"

#define FINAL_BARRIER 0x2008
#define DARTS_RT_ALIVE 0x6c

int test_message_send(int test_sig) {
    message tmp = (message) test_sig;
    return(darts_send_message(&tmp));
}

int test_message_receive(message exp) {
    message tmp;
    darts_receive_message(&tmp);
    while (tmp != exp) {
        printf("delaying\n");
        int i=0;
        while(i<100000000) i++;
        darts_receive_message(&tmp);
    }
    printf("correctly received message %d\n", tmp);
    return(0);
}

int test_data_send(unsigned num) {
    mailbox_t tmp_data;
    darts_fill_mailbox(&tmp_data, STATUS, sizeof(unsigned), NM_REQUEST_SU_PROVIDE);
    darts_unsigned_convert_to_data(num, &(tmp_data.data));
    return(darts_send_data(&tmp_data));
}

int test_data_receive() {
    mailbox_t tmp_data;
    int tmp_message = (int) darts_receive_data(&tmp_data);
    while (tmp_message < 1) {
        printf("delaying\n");
        int i=0;
        while(i<100000000) i++;
        tmp_message = darts_receive_data(&tmp_data);
    }
    printf("received message %d and data %u\n", tmp_message, darts_data_convert_to_unsigned(&(tmp_data.data)));
}

int main(int argc, char *argv[]){

    //initialize platform
    darts_init();
    //load elf file and execute on Epiphany
    clock_t start, end;
    double cpu_time;
    start = clock();
    darts_run("e_darts_comm_test.elf");
    int i=0;
    while(i<100000000) i++;

    //test_message_send(1);
    //test_message_receive(2);
    for (int i=0; i<9; i++) {
        int result = test_data_send(i);
        printf("darts sent to SU with result %d\n", result);
    }
    test_data_receive();


    //wait for runtime to shut itself down
    //darts_wait();
    end = clock();
    cpu_time = ((double) (end-start)) / CLOCKS_PER_SEC;
    printf("eDARTS used %lf seconds\n", cpu_time);
    //close the platform
    darts_close();
    return EXIT_SUCCESS;
}
