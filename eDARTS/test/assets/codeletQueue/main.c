#include <stdlib.h>
#include <stdio.h>
#include "e-hal.h"
#include "e-loader.h"

int main(int argc, char *argv[]){
    e_platform_t platform;
    e_epiphany_t dev;

    //Initalize Epiphany device
    e_init(NULL);
    e_reset_system();//reset Epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 2, 2);

    unsigned number = 0;

    e_load("e_codQueue.elf", &dev, 0, 0, E_FALSE); // producer
    e_load("e_codQueue.elf", &dev, 0, 1, E_FALSE); // consumer

    // Set the initial value for the flags
    e_write(&dev,0,0, 0x3000,&number,sizeof(number)); // startSignal
    e_write(&dev,0,1, 0x3000,&number,sizeof(number)); // doneSignal



    e_start(&dev, 0,0);
    e_start(&dev, 0,1);
    // wait until consumer is done
    while(number != 2)
    {
        e_read(&dev,0,1,0x3000,&number,sizeof(number));
    }

    e_read(&dev,0,1,0x2228,&number,sizeof(number)); //reading result from sum
    printf ("result = %u \n",number);

    e_close(&dev);
    e_finalize();
    return EXIT_SUCCESS;
}

