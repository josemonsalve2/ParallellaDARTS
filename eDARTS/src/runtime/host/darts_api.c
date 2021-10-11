#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "e-hal.h"
#include "e-loader.h"
#include "darts_print_server.h"
#include "darts_api.h"
#include "darts_rt_params.h"


nodeMailbox_t localMailbox;
e_mem_t dartsCommSpace;

int darts_init()
{
    e_init(NULL);
    e_reset_system(); //reset epiphany
    e_get_platform_info(&platform);
    e_open(&dev, 0, 0, 4, 4);
    start_printing_server();
    e_alloc(&dartsCommSpace, COMM_QUEUE_OFFSET, sizeof(commSpace_t));
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
//returns bytes written if successful, -3 if queue full, E_ERR otherwise
//watch out for conflicts between bytes written and e_return_stat_t enum
int darts_send_message(message *signal)
{
    //bool ack;
    struct {
        unsigned head;
        unsigned tail;
        unsigned full_flag;
        unsigned op_done;
    } head_tail_flag;
    e_read(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + HEAD_OFFSET, &head_tail_flag, sizeof(unsigned) + sizeof(unsigned) + sizeof(unsigned) + sizeof(unsigned));
    printf("darts_send read head %d, tail %d, flag %d, op %d\n", head_tail_flag.head, head_tail_flag.tail, head_tail_flag.full_flag, head_tail_flag.op_done);
    if (!(head_tail_flag.full_flag) && !((head_tail_flag.head == head_tail_flag.tail) && head_tail_flag.op_done)) {
        int result = e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + INDEX_OFFSET(head_tail_flag.tail) + SIGNAL_OFFSET, signal, sizeof(message));
        printf("wrote signal at offset %x\n", NM_TO_SU_OFFSET+INDEX_OFFSET(0)+SIGNAL_OFFSET);
        if (result > 0) {
            head_tail_flag.tail = (head_tail_flag.tail + 1) % _DARTS_COMM_QUEUE_LENGTH;
            printf("darts_send sent signal with result %d, writing new tail %d at %x\n", result, head_tail_flag.tail, 0x8e000130+NM_TO_SU_OFFSET+TAIL_OFFSET);
            unsigned tmp = 1;
            e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + TAIL_OFFSET, &(head_tail_flag.tail), sizeof(unsigned));
            e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + NM_OP_OFFSET, &tmp, sizeof(unsigned));
            return(result);
        }
        //return(e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + ACK_OFFSET, &pack, sizeof(sigWithAck_t)));
    }
    else return(-3);
}

int darts_send_message_wait(message *signal)
{
    struct {
        unsigned head;
        unsigned tail;
        unsigned full_flag;
    } head_tail_flag;
    //unsigned tail;
    e_read(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + HEAD_OFFSET, &head_tail_flag, sizeof(unsigned) + sizeof(unsigned) + sizeof(unsigned));
    //should add exponential backoff in future
    //max delay
    //delay
    while (!(head_tail_flag.full_flag)) {
        /*
        usleep(delay);
        if (delay < MAX_DELAY)
        {
            delay *= 2;
        }
        */
        for (int i=0; i<100; i++) {
            __asm__ ("NOP;"
                     "NOP;"
                     "NOP;"
                     "NOP;"
                     "NOP;"
                     "NOP;"
                     "NOP;"
                     "NOP;"
                    ); //nop for delays, not exponential but will work for now
        }
        e_read(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + HEAD_OFFSET, &head_tail_flag, sizeof(unsigned) + sizeof(unsigned) + sizeof(unsigned));
    }
        //sigWithAck_t pack = {false, *(signal)};
    int result = e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + INDEX_OFFSET(head_tail_flag.tail) + SIGNAL_OFFSET, signal, sizeof(message));
    if (result > 0) {
        head_tail_flag.tail = (head_tail_flag.tail + 1) % _DARTS_COMM_QUEUE_LENGTH;
        unsigned tmp = 1;
        e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + TAIL_OFFSET, &(head_tail_flag.tail), sizeof(unsigned));
        e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + NM_OP_OFFSET, &tmp, sizeof(unsigned));
        return(result);
    }
}

//add checking as above
//returns bytes written if successful, -3 if not acked, E_ERR otherwise
//watch out for conflicts between bytes written and e_return_stat_t enum
int darts_send_data(mailbox_t* data_loc)
{
    struct {
        unsigned head;
        unsigned tail;
        unsigned full_flag;
    } head_tail_flag;
    int response;
    //bool ack;
    e_read(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + HEAD_OFFSET, &head_tail_flag, sizeof(unsigned) + sizeof(unsigned) + sizeof(unsigned));
    //e_read(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + ACK_OFFSET, &ack, sizeof(bool));
    //subtract size of unsigned so as to not overwrite the mutex nor ack on the epiphany side
    int result_1 = -5;
    unsigned head = head_tail_flag.head;
    unsigned tail = head_tail_flag.tail;
	unsigned size = data_loc->msg_header.size;
    if (!(head_tail_flag.full_flag) && tail != (head-1) && tail != (head+_DARTS_COMM_QUEUE_LENGTH-1)) { //trying one space open method
	    // data transfer size is mailbox, without ack and without empty data, so only sends header through valid data first
        result_1 = e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + INDEX_OFFSET(head_tail_flag.tail), data_loc, sizeof(mailbox_t) - sizeof(sigWithAck_t) - (MAX_PAYLOAD_SIZE - size));
    }
    else return(-3); //-3 (no space) to avoid overlap with E_OK, E_ERR, E_WARN (0, -1, -2) which e_write might return
	if (result_1 < 0) {
            return(result_1);
        }
	else {
        int result_2 = e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + INDEX_OFFSET(head_tail_flag.tail) + SIGNAL_OFFSET, &(data_loc->signal), sizeof(message));
        head_tail_flag.tail = (head_tail_flag.tail + 1) % _DARTS_COMM_QUEUE_LENGTH;
        e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + TAIL_OFFSET, &(head_tail_flag.tail), sizeof(unsigned));
        unsigned op_done = 1;
        e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + NM_OP_OFFSET, &op_done, sizeof(unsigned));
	    if (result_2 < 0) {
                return(result_2);
        }
	    else return(result_1+result_2);
	}
}

int darts_send_data_wait(mailbox_t *data_loc)
{
    struct {
        unsigned head;
        unsigned tail;
        unsigned full_flag;
    } head_tail_flag;
    int response;
    e_read(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + HEAD_OFFSET, &head_tail_flag, sizeof(unsigned) + sizeof(unsigned) + sizeof(unsigned));
    //subtract size of unsigned so as to not overwrite the mutex nor ack on the epiphany side
    int result_1 = -5;
	unsigned size = data_loc->msg_header.size;
    while (!(head_tail_flag.full_flag)) {
        /*
        usleep(delay);
        if (delay < MAX_DELAY)
        {
            delay *= 2;
        }
        */
        // I think I added this accidentally; should be epiphany side
        for (int i=0; i<100; i++) {
            __asm__ ("NOP;"
                     "NOP;"
                     "NOP;"
                     "NOP;"
                     "NOP;"
                     "NOP;"
                     "NOP;"
                     "NOP;"
                    ); //nop for delays, not exponential but will work for now
        }
        e_read(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + HEAD_OFFSET, &head_tail_flag, sizeof(unsigned) + sizeof(unsigned) + sizeof(unsigned));
    }
    result_1 = e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + INDEX_OFFSET(head_tail_flag.tail), data_loc, sizeof(mailbox_t) - sizeof(sigWithAck_t) - (MAX_PAYLOAD_SIZE - size));
	if (result_1 < 0) {
            return(result_1);
        }
	else {
        int result_2 = e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + INDEX_OFFSET(head_tail_flag.tail) + SIGNAL_OFFSET, &(data_loc->signal), sizeof(message));
        head_tail_flag.tail = (head_tail_flag.tail + 1) % _DARTS_COMM_QUEUE_LENGTH;
        e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + TAIL_OFFSET, &(head_tail_flag.tail), sizeof(unsigned));
        unsigned op_done = 1;
        e_write(&dartsCommSpace, 0, 0, NM_TO_SU_OFFSET + NM_OP_OFFSET, &op_done, sizeof(unsigned));
	    if (result_2 < 0) {
                return(result_2);
        }
	    else return(result_1+result_2);
	}
}

// need to add generic tp closure to header definition and such
// make sure to push to TP queue with correct arguments, epiphany will probably have to repackage it to the proper struct
int darts_invoke_TP(void* closure)
{

}

// does not edit head / set op_done
message darts_receive_message(message *signal)
{
    unsigned head;
    e_read(&dartsCommSpace, 0, 0, SU_TO_NM_OFFSET + HEAD_OFFSET, &head, sizeof(unsigned));
    e_read(&dartsCommSpace, 0, 0, SU_TO_NM_OFFSET + INDEX_OFFSET(head) + SIGNAL_OFFSET, (message *) signal, sizeof(message));
    return(*(signal));
}

//sets ack byte intrinsically
message darts_receive_data(mailbox_t *mailbox)
{
    struct {
        unsigned head;
        unsigned tail;
        unsigned full_flag;
    } head_tail_flag;
    int response;
    e_read(&dartsCommSpace, 0, 0, SU_TO_NM_OFFSET + HEAD_OFFSET, &head_tail_flag, sizeof(unsigned) + sizeof(unsigned) + sizeof(unsigned));
    unsigned head = head_tail_flag.head;
    unsigned tail = head_tail_flag.tail;
    unsigned flag = head_tail_flag.full_flag;
    printf("darts_receive sees head %u tail %u and flag %u, reads %u\n", head, tail, flag, ((head != tail) || (head == tail && flag)));
    if ((head != tail) || (head == tail && flag)) {
        e_read(&dartsCommSpace, 0, 0, SU_TO_NM_OFFSET + INDEX_OFFSET(head), (mailbox_t *) mailbox, sizeof(mailbox_t));
        head = (head + 1) % _DARTS_COMM_QUEUE_LENGTH;
        e_write(&dartsCommSpace, 0, 0, SU_TO_NM_OFFSET + HEAD_OFFSET, &head, sizeof(unsigned));
        unsigned op_done = 1;
        e_write(&dartsCommSpace, 0, 0, SU_TO_NM_OFFSET + NM_OP_OFFSET, &op_done, sizeof(unsigned));
        return(mailbox->signal);
    }
    else {
        return(-1);
    }
}

//helper function to fill mailbox stuff without having to know the names of the fields
void darts_fill_mailbox(mailbox_t *mailbox, messageType type, unsigned size, message signal)
{
    mailbox->msg_header.msg_type = type;
    mailbox->msg_header.size = size;
    mailbox->signal = signal;
}
/*
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
*/

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
