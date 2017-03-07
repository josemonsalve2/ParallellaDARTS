#include <stdlib.h>
#include <stdio.h>
#include <e-hal.h>
#include "common.h"
void init_rt (e_epiphany_t* dev, e_platform_t* platform)
{
    //Initalize Epiphany device
    e_init(NULL);
    e_reset_system();//reset Epiphany
    e_get_platform_info(platform);
    e_open(dev, 0, 0, 4, 4);
}

void load_rt (e_epiphany_t* dev)
{
    int i;
    //Load program to cores and run
    //Load SU
    e_load("e_SU.elf", dev, SU_ROW, SU_COL, E_FALSE);
    //Load CUs
    for ( i = 0; i < NUM_CU; ++i)
    {
        e_load("e_CU.elf", dev, (i+1)/4, (i+1)%4, E_FALSE);
    }
    
}

void signal_all_cores_rt (e_epiphany_t *dev, unsigned int value)
{
    int i;
    //Hold the start
    for ( i = 0; i < NUM_CU + 1; ++i)
    {
        e_write(dev, i/4, i%4, RT_START_SIGNAL, &value, sizeof(value));
    }
}

/**
 * @brief Set up initial values for rt things
 * 
 */
void config_rt (e_epiphany_t* dev)
{
    int i, value = 1;
    signal_all_cores_rt(dev, 1);
    //Set CU flags inside the SU;
    for ( i = 0; i < NUM_CU; ++i)
    {
         e_write(dev, SU_COL, SU_ROW, SU_CU_SIGNALS + i*sizeof(unsigned), &value, sizeof(value));
    }
    
}

void start_rt (e_epiphany_t *dev)
{
    int i;
    //start all the cores
    for ( i = 0; i < NUM_CU + 1; ++i)
    {
        e_start(dev, i/4, i%4);
    }
    signal_all_cores_rt(dev, 0);
}

void end_rt (e_epiphany_t *dev)
{
    //Close down Epiphany device
    e_close(dev);
    e_finalize();
}

int main(int argc, char *argv[]){
    
    //Variables
    unsigned int val = 0;
    int done,i;
    
    //RT
    e_epiphany_t dev;
    e_platform_t platform;
    
    init_rt(&dev, &platform);
    load_rt(&dev);
    config_rt(&dev);
    start_rt(&dev);

    // Flag from CU telling that it is done
    done = 0;
    //Check if all cores are done
    while(done == 0){
        //val = 0;
        e_read(&dev, SU_COL, SU_ROW, SU_DONE_ADDR, &done, sizeof(done));
    }
    
    // Get result. this is temp
    for( i = 0; i < NUM_CU; ++i)
    {
        e_read(&dev, (i+1)/4, (i+1)%4, 0x2228, &val, sizeof(val));
        printf("Sum(%d): %u!\n",i,val);
    }
    fflush(stdout);

    end_rt (&dev);

    return EXIT_SUCCESS;
}
