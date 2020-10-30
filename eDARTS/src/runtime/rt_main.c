#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <e-hal.h>
#include "darts_print_server.h"

#define FINAL_BARRIER 0x2008


int main(int argc, char *argv[]){
    e_platform_t platform;
    e_epiphany_t dev;

    //Initalize Epiphany device
    e_init(NULL);
    e_reset_system();//reset Epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 4, 4);

    start_printing_server();

    e_load_group("e_darts_rt_init.elf", &dev, 0, 0, 4, 4, E_FALSE);
    printf("Group loaded \n");

    e_start_group(&dev);




    //here we need to check the rt_alive signal in the SU assuming its the last to turn off. once that's done
    //we can close out everything. use a delay function here for testing
    usleep(1e8);


    usleep(1e5);
    stop_printing_server();
    e_close(&dev);
    e_finalize();
    return EXIT_SUCCESS;
}

