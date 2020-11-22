#include <stdbool.h>
#include "e_darts_mailbox.h"

//Address of mailbox: 8e000138
nodeMailbox_t _dartsNodeMailbox __attribute__ ((section(".nodeMailbox")));

message e_darts_receive_signal()
{
    return _dartsNodeMailbox.NMtoSU.signal;
}

message e_darts_receive_data(mailbox_t *loc)
{
    loc->msg_header = _dartsNodeMailbox.NMtoSU.msg_header;
    unsigned size = loc->msg_header.size;
    for(int i=0; i<size; i++) {
        loc->data[i] = _dartsNodeMailbox.NMtoSU.data[i];
    } 
    loc->signal = _dartsNodeMailbox.NMtoSU.signal;
    _dartsNodeMailbox.NMtoSU.ack = true;
    return(loc->signal);
}

int e_darts_send_signal(message *signal)
{
    if (_dartsNodeMailbox.SUtoNM.ack) {
        _dartsNodeMailbox.SUtoNM.signal = *(signal);
	_dartsNodeMailbox.SUtoNM.ack = 0;
    } else return -1; //not acked
}

int e_darts_send_data(mailbox_t *loc)
{
    if (_dartsNodeMailbox.SUtoNM.ack) {
        loc->ack = false; //make sure ack is sent as false
        _dartsNodeMailbox.SUtoNM = *(loc);
    } else return -1; //not acked

}

void e_darts_set_ack(bool ack)
{
    _dartsNodeMailbox.NMtoSU.ack = ack;
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
    type_array[2] = (unsigned short)(code & 0x00f0);
    type_array[1] = (unsigned short)(code & 0x0f00);
    type_array[0] = (unsigned short)(code & 0xf000);
}

