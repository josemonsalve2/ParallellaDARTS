#include <stdlib.h>
#include <stdio.h>
#include <e-hal.h>  // Epiphany Hardware Abstraction Layer... or Epiphany Host Library
#include "common.h"

int main(int argc, char *argv[]){
        e_platform_t platform;
        e_epiphany_t dev;       // declares an eCore workgroup

        int done;                                              //declare done flag
        unsigned int val;

        //Initalize Epiphany device
        e_init(NULL);                      
        e_get_platform_info(&platform);
        e_reset_system();                                      //reset Epiphany

        e_open(&dev, 0, 0, 1, 2); // deines the eCore workgroup as (0,0) and (0,1)

        done = 0;
        val = 1;

        /* Core map (row, column):
         * (0,0) (0,1) (0,2) (0,3)
         * (1,0) (1,1) (1,2) (1,3)
         * (2,0) (2,1) (2,2) (2,3)
         * (3,0) (3,1) (3,2) (3,3)
         */

        //Load program to cores and don't start yet (E_FALSE)
        e_load("e_SU.srec", &dev, 0, 0, E_FALSE);
        e_load("e_CU.srec", &dev, 0, 1, E_FALSE);

        //Write val to the CU's internal memory address 0x2004 (workFlag)
        e_write(&dev,0,1,(DATA_OFFSET + 0x0004),&val,sizeof(val));
        
        //Start program execution
        e_start(&dev,0,0);
        e_start(&dev,0,1);
        //e_start(&dev); //Start program execution on all cores

        //Check if all cores are done
        while(1) {
//                val = 0;
//
//                //Read val from the SU's internal memory address 0x2004 (never initialized)
//                e_read(&dev,0,0,(DATA_OFFSET + 0x0004),&val,sizeof(val));
//printf("SU 0x2004: %d.\n", val);

                val = 0;

                //Read val from the CU's internal memory address 0x2028 (value)
                e_read(&dev,0,1,(DATA_OFFSET + 0x0028),&val,sizeof(val));
printf("CU 0x2028 (value): %d.\n", val);

                //Reset done flag
                done=0;

                //Read done flag from CU's interal memory address 0x2000 (done)
                e_read(&dev, 0, 1, (DATA_OFFSET + 0x0000), &done, sizeof(int));
printf("CU 0x2000 (done): %d.\n", done);

                //If done, break control loop
                if(done==1) break;
        }

        //Reset value
        val = 0;

        //Read value from the CU's internal memory address 0x2028
        e_read(&dev, 0, 1, (DATA_OFFSET + 0x0028), &val, sizeof(int));

        //Print result
        printf("Sum: %d!\n",val);
        fflush(stdout);

        //Close down Epiphany device
        e_close(&dev);  // frees eCore workgroup
        e_finalize();

        return EXIT_SUCCESS;
}

