#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <e-hal.h>
#include "darts_print_server.h"
#include "darts_api.h"


int darts_init()
{
    e_init(NULL);
    e_reset_system(); //reset epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 4, 4);
    start_printing_server();
}

// non blocking; should change this later to allow the elf file to be independently selected
// add error checking
void darts_run()
{
    e_load_group("e_darts_rt_init.elf", &dev, 0, 0, 4, 4, E_FALSE);
    printf("Group loaded \n");
    e_start_group(&dev);
    usleep(1e6);
}

// wait for the runtime to close out
void darts_wait()
{
    unsigned number = 1;
    for(int i=0; i<16; i++) { //wait for all cores to have darts_rt_alive = 0
        while(number != 0) {
            number = 1;
            e_read(&dev, i/4, i%4, DARTS_RT_ALIVE, &number, sizeof(number));
        }
    }

}

void darts_close()
{
    usleep(1e5);
    stop_printing_server();
    e_close(&dev);
    e_finalize();
}

// add SUid as arg
int darts_send_message(msg message)
{

}

int darts_receive_message(msg message)
{

}

// need to add generic tp closure to header definition and such
int darts_invoke_TP(void *data, unsigned size)
{


}

// include header struct in darts_api.h
int darts_send_data(void *data, unsigned size, header ) 
{

}

int darts_receive_data(void *local)
{

}
