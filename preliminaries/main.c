#include <stdlib.h>
#include <stdio.h>
#include <e-hal.h>
#include "common.h"

int main(int argc, char *argv[]){
        e_platform_t platform;
        e_epiphany_t dev;

        int done;

        //Initalize Epiphany device
        e_init(NULL);                      
        e_reset_system();                                      //reset Epiphany
        e_get_platform_info(&platform);
        e_open(&dev, 0, 0, 2, 2);

        unsigned int val =1;

        //Load program to cores and run
        e_load("e_SU.srec", &dev, 0, 0, E_FALSE);
        e_load("e_CU.srec", &dev, 0, 1, E_FALSE);

        e_write(&dev,0,1,0x2004,&val,sizeof(val));
        

        e_start(&dev,0,0);
        e_start(&dev,0,1);
        //Check if all cores are done
        while(1){    
                val = 0;
                e_read(&dev,0,0,0x2004,&val,sizeof(val));
                val = 0;
                e_read(&dev,0,1,0x2008,&val,sizeof(val));
                done=0;
                e_read(&dev, 0, 1, 0x2000, &done, sizeof(int));
                if(done==1){
                        break;
                }
        }
        val = 0;
        e_read(&dev, 0, 1, 0x2028, &val, sizeof(int));

        printf("Sum: %d!\n",val);
        fflush(stdout);
        //Close down Epiphany device
        e_close(&dev);
        e_finalize();

        return EXIT_SUCCESS;
}

