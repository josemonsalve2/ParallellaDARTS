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
    darts_run("e_darts_vector_dotProduct_test.elf");

    usleep(1000);
    //wait for runtime to shut itself down
    darts_wait();
    //close the platform
    darts_close();
    return EXIT_SUCCESS;
}
