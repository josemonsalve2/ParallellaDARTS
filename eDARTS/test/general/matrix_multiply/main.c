#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "e-hal.h"
#include "darts_print_server.h"
#include "darts_api.h"

#define FINAL_BARRIER 0x2008
#define DARTS_RT_ALIVE 0x6c


int main(int argc, char *argv[]){

    //initialize platform
    darts_init();
    //load elf file and execute on Epiphany
    clock_t start, end;
    double cpu_time;
    start = clock();
    darts_run("e_darts_matrix_multiply_test.elf");

    //wait for runtime to shut itself down
    darts_wait();
    end = clock();
    cpu_time = ((double) (end-start)) / CLOCKS_PER_SEC;
    printf("eDARTS used %lf seconds\n", cpu_time);
    //close the platform
    darts_close();
    return EXIT_SUCCESS;
}
