#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <e-hal.h>
#include "darts_print_server.h"

#define FINAL_BARRIER 0x2008
#define DARTS_RT_ALIVE 0x6c


int main(int argc, char *argv[]){
    e_platform_t platform;
    e_epiphany_t dev;

    //Initalize Epiphany device
    e_init(NULL);
    e_reset_system();//reset Epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 4, 4);

    start_printing_server();
    
    unsigned number = 1;

    e_load_group("e_darts_rt_init.elf", &dev, 0, 0, 4, 4, E_FALSE);
    printf("Group loaded \n");

    e_start_group(&dev);

    usleep(1e6);
    /*
    for(int i=0; i<16; i++) { //wait for all cores to have darts_rt_alive = 0
        while(number != 0) {
            number = 1;
            e_read(&dev, i/4, i%4, DARTS_RT_ALIVE, &number, sizeof(number));
        }
    }
    */
    while(number != 0) {
        e_read(&dev, 0, 0, DARTS_RT_ALIVE, &number, sizeof(number));
    }

    usleep(1e5);
    stop_printing_server();

    e_close(&dev);
    e_finalize();
    return EXIT_SUCCESS;
}

