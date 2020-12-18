#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "e-hal.h"
#include "e-loader.h"
#include "darts_print_server.h"


#define SEQUENTIALIZED_RESULT_LOCATION 0x2220
#define UNSAFE_RESULT_LOCATION 0x2224
#define INIT_SIGNAL 0x2004
#define FINAL_SIGNAL 0x2008
#define BARRIER_LOCATION 0x200C

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

    printf("This test will execute the e_darts_barrier.elf "
    		"on 16 cores.\n Each core will add one on its "
    		"turn based on a thread number and this operation. "
    		"gets repeated 1000 times expecting a value of 16000\n");

    e_load_group("e_darts_barrier_test.elf", &dev, 0, 0, 4, 4, E_FALSE);

    // Set the initial value for the flags
    e_write(&dev, 0, 0, INIT_SIGNAL, &number, sizeof(number)); // startSignal
    e_write(&dev, 0, 0, FINAL_SIGNAL, &number, sizeof(number)); // startSignal
    e_start_group(&dev);

    while(number != 1) {
        e_read(&dev,0,0,FINAL_SIGNAL,&number,sizeof(number));
    }

     e_read(&dev,0,0,SEQUENTIALIZED_RESULT_LOCATION,&number,sizeof(number)); //reading result from safe sum
     printf ("SEQUENTIALIZED SUM RESULT = %d \n",number);

     e_read(&dev,0,0,UNSAFE_RESULT_LOCATION,&number,sizeof(number)); //reading result from unsafe sum
     printf ("UNSAFE SUM RESULT = %d \n",number);

     usleep(10e5);
     stop_printing_server();

     e_close(&dev);
     e_finalize();
    return EXIT_SUCCESS;
}

