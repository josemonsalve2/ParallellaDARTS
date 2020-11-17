

#define E_DARTS_OK 0


e_platform_t platform;
e_epiphany_t dev;

typedef enum messages {
   // get messages from mailbox.h 
} msg;

int e_darts_init();
/*
{
    e_init(NULL);
    e_reset_system(); //reset epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 4, 4);
    start_printing_server();
}
*/

// non blocking; should change this later to allow the elf file to be independently selected
// add error checking
void e_darts_run();
/*
{
    e_load_group("e_darts_rt_init.elf", &dev, 0, 0, 4, 4, E_FALSE);
    printf("Group loaded \n");
    e_start_group(&dev);
    usleep(1e6);
}
*/
// wait for the runtime to close out
void e_darts_wait();
/*
{
    unsigned number = 1;
    for(int i=0; i<16; i++) { //wait for all cores to have darts_rt_alive = 0
        while(number != 0) {
            number = 1;
            e_read(&dev, i/4, i%4, DARTS_RT_ALIVE, &number, sizeof(number));
        }
    }

}
*/

void e_darts_close();
/*
{
    usleep(1e5);
    stop_printing_server();
    e_close(&dev);
    e_finalize();
}
*/

int e_darts_send_message(msg message);

// need to add generic tp closure to header definition and such
int e_darts_invoke_TP(tp_closure_t closure);
