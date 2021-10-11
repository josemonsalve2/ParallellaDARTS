#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "e-hal.h"
#include "e-loader.h"
#include "darts_print_server.h"

#define INIT_FLAG 0x2000
#define FINI_FLAG 0x2004

int main(int argc, char *argv[]){
    e_platform_t platform;
    e_epiphany_t dev;

    //Initalize Epiphany device
    e_init(NULL);
    e_reset_system();//reset Epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 4, 4);

    start_printing_server();

    unsigned number = 0;

    e_load_group("e_syncSlot_test.elf", &dev, 0, 0, 4, 4, E_FALSE); // producer


    // Set the initial value for the flags
    e_write(&dev,0,0, INIT_FLAG,&number,sizeof(number)); // startSignal
    e_write(&dev,0,0, FINI_FLAG,&number,sizeof(number)); // doneSignal



    e_start_group(&dev);
    // wait until consumer is done
    while(number != 1)
    {
        e_read(&dev,0,0,FINI_FLAG,&number,sizeof(number));
    }


    usleep(1e5);
    stop_printing_server();

    e_close(&dev);
    e_finalize();
    return EXIT_SUCCESS;
}

