#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "e-hal.h"
#include "e-loader.h"
#include "darts_print_server.h"
#include "darts_api.h"


nodeMailbox_t localMailbox;
e_mem_t nodeMailbox;

int darts_init()
{
    e_init(NULL);
    e_reset_system(); //reset epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 4, 4);
    start_printing_server();
    e_alloc(&nodeMailbox, 0x00000134, sizeof(nodeMailbox_t));
    // 0x00000134 is the offset to the mailbox from beginning of DRAM 0x8e
}

// non blocking; should change this later to allow the elf file to be independently selected
// add error checking
void darts_run(char* elf_file_name)
{
    e_load_group(elf_file_name, &dev, 0, 0, 4, 4, E_FALSE);
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

int darts_send_message_wait(message *signal)
{
    bool ack;
    e_read(&nodeMailbox, 0, 0, NM_TO_SU_OFFSET + ACK_OFFSET, &ack, sizeof(bool));
    if (ack) {
        sigWithAck_t pack = {false, *(signal)};
        return(e_write(&nodeMailbox, 0, 0, NM_TO_SU_OFFSET + ACK_OFFSET, &pack, sizeof(sigWithAck_t)));
    }
    else {
        while (!ack) {
            e_read(&nodeMailbox, 0, 0, NM_TO_SU_OFFSET + ACK_OFFSET, &ack, sizeof(bool));
        }
        sigWithAck_t pack = {false, *(signal)};
        return(e_write(&nodeMailbox, 0, 0, NM_TO_SU_OFFSET + ACK_OFFSET, &pack, sizeof(sigWithAck_t)));
    }
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

int darts_send_data_wait(mailbox_t *data_loc)
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
    else {
        while (!ack) {
            e_read(&nodeMailbox, 0, 0, NM_TO_SU_OFFSET + ACK_OFFSET, &ack, sizeof(bool));
        }
        return(e_write(&nodeMailbox, 0, 0, NM_TO_SU_OFFSET, (mailbox_t *) data_loc, sizeof(mailbox_t)-sizeof(unsigned)));
    }
}

// need to add generic tp closure to header definition and such
// make sure to push to TP queue with correct arguments, epiphany will probably have to repackage it to the proper struct
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
message darts_receive_data(mailbox_t *mailbox)
{
    //subtract size of unsigned so that darts_mutex value is pulled, dont need it just saves space
    e_read(&nodeMailbox, 0, 0, SU_TO_NM_OFFSET, (mailbox_t *) mailbox, sizeof(mailbox_t)); //probably don't have to transfer lock
    darts_set_ack(true);
    return(localMailbox.SUtoNM.signal);
}

//helper function to fill mailbox stuff without having to know the names of the fields
void darts_fill_mailbox(mailbox_t *mailbox, messageType type, unsigned size, message signal)
{
    mailbox->msg_header.msg_type = type;
    mailbox->msg_header.size = size;
    mailbox->signal = signal;
}

int darts_set_ack(bool ack)
{
    bool ack_val = ack;
    return(e_write(&nodeMailbox, 0, 0, SU_TO_NM_OFFSET + ACK_OFFSET, &ack_val, sizeof(bool)));
}

bool darts_get_ack()
{
    bool ack;
    e_read(&nodeMailbox, 0, 0, SU_TO_NM_OFFSET + ACK_OFFSET, &ack, sizeof(bool));
    return(ack);
}

int darts_data_convert_to_int(char *data)
{
    int_converter char_to_int;
    char_to_int.raw[0] = data[0];
    char_to_int.raw[1] = data[1];
    char_to_int.raw[2] = data[2];
    char_to_int.raw[3] = data[3];
    return(char_to_int.processed);
}

unsigned darts_data_convert_to_unsigned(char *data)
{
    unsigned_converter char_to_uns;
    char_to_uns.raw[0] = data[0];
    char_to_uns.raw[1] = data[1];
    char_to_uns.raw[2] = data[2];
    char_to_uns.raw[3] = data[3];
    return(char_to_uns.processed);
}

void darts_int_convert_to_data(int input, char *data)
{
    int_converter int_to_char;
    int_to_char.processed = input;
    data[0] = int_to_char.raw[0];
    data[1] = int_to_char.raw[1];
    data[2] = int_to_char.raw[2];
    data[3] = int_to_char.raw[3];
}

void darts_unsigned_convert_to_data(unsigned input, char *data)
{
    unsigned_converter uns_to_char;
    uns_to_char.processed = input;
    data[0] = uns_to_char.raw[0];
    data[1] = uns_to_char.raw[1];
    data[2] = uns_to_char.raw[2];
    data[3] = uns_to_char.raw[3];
}

//array of counts of args in following order: int, unsigned, char, float
unsigned short darts_args_encoding(unsigned short *type_array)
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
