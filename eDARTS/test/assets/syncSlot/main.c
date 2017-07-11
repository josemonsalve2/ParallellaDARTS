#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <e-hal.h>
#include "darts_print_server.h"

int main(int argc, char *argv[]){
    e_platform_t platform;
    e_epiphany_t dev;

    //Initalize Epiphany device
    e_init(NULL);
    e_reset_system();//reset Epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 2, 2);

    start_printing_server();

    unsigned number = 0;

    e_load("e_syncSlot_test.elf", &dev, 0, 0, E_FALSE); // producer


    // Set the initial value for the flags
//    e_write(&dev,0,0, 0x3000,&number,sizeof(number)); // startSignal
//    e_write(&dev,0,1, 0x3000,&number,sizeof(number)); // doneSignal



    e_start_group(&dev);
//    // wait until consumer is done
//    while(number != 2)
//    {
//        e_read(&dev,0,1,0x3000,&number,sizeof(number));
//    }
//
//    e_read(&dev,0,1,0x2228,&number,sizeof(number)); //reading result from sum
//    printf ("result = %d \n",number);


    usleep(10e5);
    stop_printing_server();

    e_close(&dev);
    e_finalize();
    return EXIT_SUCCESS;
}

