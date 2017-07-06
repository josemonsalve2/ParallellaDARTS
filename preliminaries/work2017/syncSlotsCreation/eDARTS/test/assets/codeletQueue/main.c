#include <stdlib.h>
#include <stdio.h>
#include <e-hal.h>
#include "common.h"

// int main(int argc, char *argv[]){
//         e_platform_t platform;
//         e_epiphany_t dev;
// 
//         int done;
// 
//         //Initalize Epiphany device
//         e_init(NULL);
//         e_reset_system();//reset Epiphany
//         e_get_platform_info(&platform);
//         e_open(&dev, 0, 0, 2, 2);
// 
// 	//initiate variable 0x2000. of CU
//         unsigned int val = 2;
//         int sync = 0;
// 
//         //Load program to cores and run
//         e_load("e_SU.elf", &dev, 0, 0, E_FALSE);
//         e_load("e_CU.elf", &dev, 0, 1, E_FALSE);
//     
//         //Initialize variables before starting the kernel
//         e_write(&dev,0,0,0x2000,&val,sizeof(val));
//         e_write(&dev,0,1,0x2000,&val,sizeof(val));
// 
//         //Start the kernel
//         e_start(&dev,0,0);
//         e_start(&dev,0,1);
//         
//         // We wait for the kernel to init
//         while(sync != 4){
//             sync = 0;
//             done = 0;
//             e_read(&dev, 0, 0, 0x2000, &done, sizeof(int));
//             sync = sync + done;
//             done = 0;
//             e_read(&dev, 0, 1, 0x2000, &done, sizeof(int));
//             sync = sync + done;
//             printf("%d",sync);
//         }
//         // We tell the kernels they can start
//         e_write(&dev,0,0,0x2000,&val,sizeof(val));
//         e_write(&dev,0,1,0x2000,&val,sizeof(val));
// 
// 	// Flag from CU telling that it is done
//         done=0;
//         //Check if all cores are done
//         while(1){
//                 //val = 0;
//                 //e_read(&dev,0,0,0x2004,&val,sizeof(val));
//                 //val = 0;
//                 //e_read(&dev,0,1,0x2008,&val,sizeof(val));
//                 e_read(&dev, 0, 1, 0x2004, &done, sizeof(int));
//                 if(done==2){
//                         break;
//                 }
//         }
//         val = 0;
//         e_read(&dev, 0, 1, 0x2228, &val, sizeof(int));
// 
// 	unsigned int doneVal;
// 
//         printf("Sum: %d!\n",val);
// 
//         fflush(stdout);
//         //Close down Epiphany device
//         e_close(&dev);
//         e_finalize();
// 
//         return EXIT_SUCCESS;
// }

int main(int argc, char *argv[]){
    e_platform_t platform;
    e_epiphany_t dev;
    
    //Initalize Epiphany device
    e_init(NULL);
    e_reset_system();//reset Epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 2, 2);
    
    unsigned number = 0;
    
    e_load("e_SU.elf", &dev, 0, 1, E_FALSE);
    e_load("e_CU.elf", &dev, 0, 0, E_FALSE);
    
    e_write(&dev,0,0, 0x3000,&number,sizeof(number));
    e_write(&dev,0,1, 0x3000,&number,sizeof(number));
    
    
    e_start(&dev, 0,0);
    e_start(&dev, 0,1);
    
    
    while(number == 0)
    {
        e_read(&dev,0,0,0x3000,&number,sizeof(number));
    }
    printf ("number %d = %d \n",1,number);
    number = 0;
    while(number ==0)
    {
        e_read(&dev,0,1,0x3000,&number,sizeof(number));
    }
    printf ("number %d = %d \n",2,number);
    
    e_close(&dev);
    e_finalize();
    return EXIT_SUCCESS;
}

