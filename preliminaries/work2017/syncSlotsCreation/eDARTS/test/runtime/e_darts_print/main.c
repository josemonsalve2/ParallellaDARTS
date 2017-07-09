#include <stdlib.h>
#include <stdio.h>
#include <e-hal.h>
#include "darts_print_server.h"

int main(int argc, char *argv[]){
    e_platform_t platform;
    e_epiphany_t dev;
    
    //Initalize Epiphany device
    e_init(NULL);
    e_reset_system();//reset Epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 4, 4);
     
    start_printing_server();
    
//     unsigned number = 0;
//     
//     e_load_group("e_darts_mutex_test.elf", &dev, 0, 0, 4, 4, E_FALSE);
//      
//     // Set the initial value for the flags
//     e_write(&dev,0,0, INIT_SIGNAL,&number,sizeof(number)); // startSignal
//     e_start_group(&dev);
//     
//     while(number != 16)
//     {
//         e_read(&dev,0,0,FINAL_BARRIER,&number,sizeof(number));
//     }
//      
//      e_read(&dev,0,0,SAFE_SUM_RESULT,&number,sizeof(number)); //reading result from safe sum
//      printf ("SAFE SUM RESULT = %d \n",number);
// 
//      e_read(&dev,0,0,UNSAFE_SUM_RESULT,&number,sizeof(number)); //reading result from unsafe sum
//      printf ("UNSAFE SUM RESULT = %d \n",number);
// 
//      e_read(&dev,0,0,FINAL_TIMER_RESULT,&number,sizeof(number)); //reading result from unsafe sum
//      printf ("LAPSED NUMBER OF CYCLES = %d \n",number);
// 
//     
    stop_printing_server();
     e_close(&dev);
     e_finalize();
    return EXIT_SUCCESS;
}

