#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <e-hal.h>
#include "darts_print_server.h"
#include "darts_api.h"

/*
 * from darts_api.h for convenience
E_DARTS_OK 0
BASE_OFFSET 0x00000000
MAX_PAYLOAD_SIZE 0x7f //127, so alignment works with the bool
MAILBOX_ADDRESS 0x8e000138 //based on print statement
SU_TO_NM_OFFSET 0x0
HEADER_OFFSET 0x0
PAYLOAD_OFFSET 0xc
ACK_OFFSET (PAYLOAD_OFFSET + MAX_PAYLOAD_SIZE)
SIGNAL_OFFSET (ACK_OFFSET + 0x1)
NM_TO_SU_OFFSET (SIGNAL_OFFSET + 0x4)
*/

nodeMailbox_t localMailbox;
e_mem_t nodeMailbox;

int darts_init()
{
    e_init(NULL);
    e_reset_system(); //reset epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 4, 4);
    start_printing_server();
    e_alloc(&nodeMailbox, BASE_OFFSET, sizeof(nodeMailbox_t));
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

//add checking to make sure its not overwriting a message that the SU hasn't received yet
//returns bytes written if successful, -1 if not acked, E_ERR otherwise
//watch out for conflicts between bytes written and e_return_stat_t enum
int darts_send_message(message *signal)
{
    bool ack;
    e_read(&nodeMailbox, 0, 0, NM_TO_SU_OFFSET + ACK_OFFSET, &ack, sizeof(bool));
    if (ack) {
        sigWithAck_t pack = {false, *(signal)};
        return(e_write(&nodeMailbox, 0, 0, NM_TO_SU_OFFSET + ACK_OFFSET, &pack, sizeof(sigWithAck_t)));
    }
    else return(-1);
}

//add checking as above
//returns bytes written if successful, -1 if not acked, E_ERR otherwise
//watch out for conflicts between bytes written and e_return_stat_t enum
int darts_send_data(mailbox_t* data_loc)
{
    int response;
    bool ack;
    e_read(&nodeMailbox, 0, 0, NM_TO_SU_OFFSET + ACK_OFFSET, &ack, sizeof(bool));
    //subtract size of unsigned so as to not overwrite the mutex nor ack on the epiphany side
    if (ack) {
        //make sure ack isn't true on arrival, should be sent as false
        data_loc->ack = false;
        return(e_write(&nodeMailbox, 0, 0, NM_TO_SU_OFFSET, (mailbox_t *) data_loc, sizeof(mailbox_t)-sizeof(unsigned)));
    }
    else return(-1);
}

// need to add generic tp closure to header definition and such
int darts_invoke_TP(void* closure)
{

}

// does not set ack byte
message darts_receive_message(message *signal)
{
    e_read(&nodeMailbox, 0, 0, SU_TO_NM_OFFSET + SIGNAL_OFFSET, (message *) signal, sizeof(message));
    return(localMailbox.SUtoNM.signal);
}

//sets ack byte intrinsically
message darts_receive_data(mailbox_t* mailbox)
{
    //subtract size of unsigned so that darts_mutex value is pulled, dont need it just saves space
    e_read(&nodeMailbox, 0, 0, SU_TO_NM_OFFSET, (mailbox_t *) mailbox, sizeof(mailbox_t)-sizeof(unsigned));
    darts_set_ack(true);
    return(localMailbox.SUtoNM.signal);
}

int darts_set_ack(bool ack)
{
    bool ack_val = ack;
    e_write(&nodeMailbox, 0, 0, SU_TO_NM_OFFSET + ACK_OFFSET, &ack_val, sizeof(bool));
}

//array of counts of args in following order: int, unsigned, char, float
short darts_args_encoding(unsigned *type_array)
{
    // max is 15 for each
    unsigned num_int = type_array[0];
    unsigned num_uns = type_array[1];
    unsigned num_char = type_array[2];
    unsigned num_float = type_array[3];
    short code = (short)(num_int & 0x0000000f);
    code = code << 4 + (short)(num_uns & 0x0000000f);
    code = code << 4 + (short)(num_char & 0x0000000f);
    code = code << 4 + (short)(num_float & 0x0000000f);
    return(code);
}
