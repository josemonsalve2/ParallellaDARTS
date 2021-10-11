#include <stdbool.h>
#include "e_darts_mailbox.h"
#include "e_darts_mutex.h"

//Address of mailbox: 8e000138
//nodeMailbox_t _dartsNodeMailbox __attribute__ ((section(".nodeMailbox")));
commSpace_t _dartsCommSpace __attribute__ ((section(".commSpace")));
darts_mutex_t __SUtoNM_mutex __attribute__ ((section("suMailMutex"))) = DARTS_MUTEX_NULL;
darts_mutex_t __NMtoSU_mutex __attribute__ ((section("nmMailMutex"))) = DARTS_MUTEX_NULL;
//darts_mutex_lock(*mutex)
//darts_mutex_unlock(*mutex)

//for now have to acquire lock and for tail access, sender has to increment tail
// key decision: when to decrement head? on first read? i think yes
// also: at this point, we can do away with ack flags?

//for debugging
void e_darts_print_SU_queue()
{
    commQueue_t *queue = &(_dartsCommSpace.SUtoNM);
    e_darts_print("SUtoNM:\n head: %d\n tail: %d\n %d %d %d %d %d %d %d %d\n", queue->head_index, queue->tail_index, \
                queue->queue[0].signal, queue->queue[1].signal, queue->queue[2].signal, queue->queue[3].signal, \
                queue->queue[4].signal, queue->queue[5].signal, queue->queue[6].signal, queue->queue[7].signal);
}

void e_darts_print_NM_queue()
{
    commQueue_t *queue = &(_dartsCommSpace.NMtoSU);
    e_darts_print("NMtoSU:\n head: %d\n tail: %d\n %d %d %d %d %d %d %d %d\n", queue->head_index, queue->tail_index, \
                queue->queue[0].signal, queue->queue[1].signal, queue->queue[2].signal, queue->queue[3].signal, \
                queue->queue[4].signal, queue->queue[5].signal, queue->queue[6].signal, queue->queue[7].signal);
    //data scan of first two slots
    //e_darts_print("NMtoSU: [1] - %x . . . %x\n", *(unsigned*)(&(queue->queue[0].data)), *(unsigned*)(&(queue->queue[0].data[124])));
}

void e_darts_comm_init()
{
    // so:
    // on incoming mailbox: when popping, increase head (modulus)
    // 			    when new push from other end, increase tail
    // 			    ** this is the problem. SU doesn't know when
    // 			    NM adds to queue, so NM would have to do it.
    // 			    This necessitates a lock for the head and tail indices
    _dartsCommSpace.SUtoNM.head_index = 0;
    _dartsCommSpace.SUtoNM.tail_index = 0; //index after final element, starts empty
    _dartsCommSpace.SUtoNM.full_flag = 0;
    _dartsCommSpace.SUtoNM.NM_op_done = 0;
    _dartsCommSpace.NMtoSU.head_index = 0;
    _dartsCommSpace.NMtoSU.tail_index = 0; //index after final element, starts empty
    _dartsCommSpace.NMtoSU.full_flag = 0;
    _dartsCommSpace.NMtoSU.NM_op_done = 0;
    for (int i=0; i<_DARTS_COMM_QUEUE_LENGTH; i++) {
        _dartsCommSpace.SUtoNM.queue[i].signal = blank;
        _dartsCommSpace.NMtoSU.queue[i].signal = blank;
    }
    //debugging
    e_darts_print("dartsCommSpace begins at %x, last signal at %x\n", &(_dartsCommSpace), &(_dartsCommSpace.NMtoSU.queue[_DARTS_COMM_QUEUE_LENGTH - 1].signal));
    e_darts_print("NMtoSU base at %x\n", &(_dartsCommSpace.NMtoSU));
    e_darts_print("First NMtoSU signal at %x\n", &(_dartsCommSpace.NMtoSU.queue[0].signal));
    e_darts_print("NMtoSU tail at %x\n", &(_dartsCommSpace.NMtoSU.tail_index));
}

/* note that this does not edit head to mark the received signal as read (it's like a peek) */
message e_darts_receive_signal()
{
    unsigned head = _dartsCommSpace.NMtoSU.head_index;
    unsigned tail = _dartsCommSpace.NMtoSU.tail_index;
    unsigned flag = _dartsCommSpace.NMtoSU.full_flag;
    //e_darts_print("e_darts_receive sees head %d, tail %d, flag %d (reads? %d)\n", head, tail, flag, ((head!=tail)||(head==tail&&flag)));
    if ((head != tail) || (head == tail && flag)) { // if not empty
        return (_dartsCommSpace.NMtoSU.queue[head].signal);
    }
    return(-1);
}

//receiving, so increment head
message e_darts_receive_data(mailbox_t *loc)
{
    unsigned head = _dartsCommSpace.NMtoSU.head_index;
    unsigned tail = _dartsCommSpace.NMtoSU.tail_index;
    unsigned flag = _dartsCommSpace.NMtoSU.full_flag;
    e_darts_print("e_darts_receive sees head %d, tail %d, flag %d (reads? %d)\n", head, tail, flag, ((head!=tail)||(head==tail&&flag)));
    if ((head == tail) && !flag) {
        e_darts_print("e_darts_receive not reading (queue empty)\n");
        return(-1); //queue empty
    }
    loc->msg_header = _dartsCommSpace.NMtoSU.queue[head].msg_header;
    unsigned size = loc->msg_header.size;
    for(int i=0; i<size; i++) {
        loc->data[i] = _dartsCommSpace.NMtoSU.queue[head].data[i];
    } 
    loc->signal = _dartsCommSpace.NMtoSU.queue[head].signal;
    _dartsCommSpace.NMtoSU.queue[head].ack = true;
    _dartsCommSpace.NMtoSU.head_index = (head + 1) % _DARTS_COMM_QUEUE_LENGTH; //increment head circularly
    // done reading, so its not full anymore (head is updated)
    if(_dartsCommSpace.NMtoSU.full_flag) {
        _dartsCommSpace.NMtoSU.full_flag = 0;
    }
    e_darts_print("e_darts_receive got signal %d\n", loc->signal);
    return(loc->signal);
}

//going to need some size checking for send functions
int e_darts_send_signal(message *signal)
{
    if (_dartsCommSpace.SUtoNM.full_flag && _dartsCommSpace.SUtoNM.NM_op_done) {
        // marked full, and op done - so NM has received, it's not actually full anymore
        _dartsCommSpace.SUtoNM.full_flag = 0;
        _dartsCommSpace.SUtoNM.NM_op_done = 0;
    }
    unsigned tail = _dartsCommSpace.SUtoNM.tail_index;
    _dartsCommSpace.SUtoNM.queue[tail].signal = *(signal);
    _dartsCommSpace.SUtoNM.tail_index = (tail + 1) % _DARTS_COMM_QUEUE_LENGTH;
    if (_dartsCommSpace.SUtoNM.head_index == _dartsCommSpace.SUtoNM.tail_index) {
        _dartsCommSpace.SUtoNM.full_flag = 1;
    }
    //_dartsCommSpace.SUtoNM.queue[tail].ack = 0;
    //darts_mutex_unlock(__SUtoNM_mutex);
    return(0);
}

void e_darts_fill_mailbox(mailbox_t *mailbox, messageType type, unsigned size, message signal)
{
    mailbox->msg_header.msg_type = type;
    mailbox->msg_header.size = size;
    mailbox->signal = signal;
}

// nonblocking
int e_darts_send_data(mailbox_t *loc)
{
    //removed ack checking
    //darts_mutex_lock(&(__SUtoNM_mutex)); //acquire lock for SU to NM
    unsigned tail = _dartsCommSpace.SUtoNM.tail_index;
    unsigned head = _dartsCommSpace.SUtoNM.head_index;
    //_DARTS_COMM_QUEUE_LENGTH
    //int space = e_darts_get_queue_space(&(_dartsCommSpace.SUtoNM));
    //if (!space) {
    if (tail != (head-1) && tail != (head+_DARTS_COMM_QUEUE_LENGTH-1)) { //trying one space open method of full
        return(-1);
    }
    _dartsCommSpace.SUtoNM.queue[tail].msg_header = loc->msg_header;
    unsigned size = loc->msg_header.size;
    for(int i=0; i<size; i++) {
        _dartsCommSpace.SUtoNM.queue[tail].data[i] = loc->data[i];
    }
    _dartsCommSpace.SUtoNM.queue[tail].signal = loc->signal;
    _dartsCommSpace.SUtoNM.tail_index = (tail + 1) % _DARTS_COMM_QUEUE_LENGTH;
    if (_dartsCommSpace.SUtoNM.head_index == _dartsCommSpace.SUtoNM.tail_index) {
        _dartsCommSpace.SUtoNM.full_flag = 1;
    }
    //_dartsCommSpace.SUtoNM.queue[tail].ack = false;
    //darts_mutex_unlock(&(__SUtoNM_mutex)); //acquire lock for SU to NM
    return((int)size);
}

int e_darts_get_queue_space(commQueue_t *queue)
{
    unsigned tail = queue->tail_index;
    unsigned head = queue->head_index;
    unsigned full_flag = queue->full_flag;
    unsigned op_done = queue->NM_op_done;
    if (full_flag && !op_done) {
        return(0);
    }
    else if (tail == head && !full_flag) {
        return (_DARTS_COMM_QUEUE_LENGTH);
    }
    else if (tail > head) {
        queue->NM_op_done = 0;
        queue->full_flag = 0;
        return (_DARTS_COMM_QUEUE_LENGTH - (tail-head));
    }
    else {
        return (head-tail);
    }
}

void e_darts_set_ack(bool ack)
{
    unsigned head = _dartsCommSpace.NMtoSU.head_index;
    _dartsCommSpace.NMtoSU.queue[head].ack = ack;
}

bool e_darts_get_ack()
{
    unsigned head = _dartsCommSpace.NMtoSU.head_index;
    return(_dartsCommSpace.NMtoSU.queue[head].ack);
}

void e_darts_int_convert_to_data(int input, char *data)
{
    int_converter int_to_data;
    int_to_data.processed = input;
    data[0] = int_to_data.raw[0];
    data[1] = int_to_data.raw[1];
    data[2] = int_to_data.raw[2];
    data[3] = int_to_data.raw[3];
}

void e_darts_unsigned_convert_to_data(unsigned input, char *data)
{
    unsigned_converter uns_to_data;
    uns_to_data.processed = input;
    data[0] = uns_to_data.raw[0];
    data[1] = uns_to_data.raw[1];
    data[2] = uns_to_data.raw[2];
    data[3] = uns_to_data.raw[3];
}

int e_darts_data_convert_to_int(char *data)
{
    int_converter data_to_int;
    data_to_int.raw[0] = data[0];
    data_to_int.raw[1] = data[1];
    data_to_int.raw[2] = data[2];
    data_to_int.raw[3] = data[3];
    return(data_to_int.processed);
}

unsigned e_darts_data_convert_to_unsigned(char *data)
{
    unsigned_converter data_to_uns;
    data_to_uns.raw[0] = data[0];
    data_to_uns.raw[1] = data[1];
    data_to_uns.raw[2] = data[2];
    data_to_uns.raw[3] = data[3];
    return(data_to_uns.processed);
}

//array of counts of args in following order: int, unsigned, char, float
unsigned short e_darts_args_encoding(unsigned short *type_array)
{
    // max is 15 for each
    unsigned short num_int = type_array[0];
    unsigned short num_uns = type_array[1];
    unsigned short num_char = type_array[2];
    unsigned short num_float = type_array[3];
    unsigned short code = (unsigned short)(num_int & 0x000f);
    code = code << 4 | (unsigned short)(num_uns & 0x000f);
    code = code << 4 | (unsigned short)(num_char & 0x000f);
    code = code << 4 | (unsigned short)(num_float & 0x000f);
    return(code);
}

void e_darts_args_decoding(unsigned short code, unsigned short *type_array)
{
    type_array[3] = (unsigned short)(code & 0x000f);
    type_array[2] = (unsigned short)((code & 0x00f0)>>4);
    type_array[1] = (unsigned short)((code & 0x0f00)>>8);
    type_array[0] = (unsigned short)((code & 0xf000)>>12);
}

