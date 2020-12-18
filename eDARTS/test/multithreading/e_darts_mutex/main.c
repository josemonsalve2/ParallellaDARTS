#include <stdlib.h>
#include <stdio.h>
#include "e-hal.h"
#include "e-loader.h"

#define SAFE_SUM_RESULT 0x2228
#define UNSAFE_SUM_RESULT 0x222C
#define INIT_SIGNAL 0x2004
#define FINAL_BARRIER 0x2008
#define FINAL_TIMER_RESULT 0x200C

int main(int argc, char *argv[]){
    e_platform_t platform;
    e_epiphany_t dev;
    
    //Initalize Epiphany device
    e_init(NULL);
    e_reset_system();//reset Epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 4, 4);
     
    unsigned number = 0;
    
    printf("This test will execute the e_darts_mutex_test.elf \
on 16 cores. \n each core will add one 1000 times using the \
e_darts_mutex lock and not using it on the same location. \
Expected result is 16000\n");
    
    e_load_group("e_darts_mutex_test.elf", &dev, 0, 0, 4, 4, E_FALSE);
     
    // Set the initial value for the flags
    e_write(&dev,0,0, INIT_SIGNAL,&number,sizeof(number)); // startSignal
    e_start_group(&dev);
    
    while(number != 16)
    {
        e_read(&dev,0,0,FINAL_BARRIER,&number,sizeof(number));
    }
     
     e_read(&dev,0,0,SAFE_SUM_RESULT,&number,sizeof(number)); //reading result from safe sum
     printf ("SAFE SUM RESULT = %d \n",number);

     e_read(&dev,0,0,UNSAFE_SUM_RESULT,&number,sizeof(number)); //reading result from unsafe sum
     printf ("UNSAFE SUM RESULT = %d \n",number);

     e_read(&dev,0,0,FINAL_TIMER_RESULT,&number,sizeof(number)); //reading result from unsafe sum
     printf ("LAPSED NUMBER OF CYCLES = %d \n",number);

     
     e_close(&dev);
     e_finalize();
    return EXIT_SUCCESS;
}

