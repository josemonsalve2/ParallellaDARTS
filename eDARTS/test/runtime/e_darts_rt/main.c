#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "e-hal.h"
#include "e-loader.h"
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

    e_load("e_darts_rt_test.elf", &dev, 0, 0, E_FALSE);

    e_start_group(&dev);

    usleep(1e5);
    stop_printing_server();
    e_close(&dev);
    e_finalize();
    return EXIT_SUCCESS;
}

