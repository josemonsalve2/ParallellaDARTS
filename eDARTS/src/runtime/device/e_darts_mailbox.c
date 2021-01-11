#include <stdbool.h>
#include "e_darts_mailbox.h"

//Address of mailbox: 8e000138
nodeMailbox_t _dartsNodeMailbox __attribute__ ((section(".nodeMailbox")));

void e_darts_node_mailbox_init()
{
    _dartsNodeMailbox.SUtoNM.signal = blank;
    _dartsNodeMailbox.NMtoSU.signal = blank;
    _dartsNodeMailbox.SUtoNM.ack = true;
    _dartsNodeMailbox.NMtoSU.ack = true;
}

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
	return(0);
    } else return(-1); //not acked
}

int e_darts_send_data(mailbox_t *loc)
{
    if (_dartsNodeMailbox.SUtoNM.ack) {
        loc->ack = false; //make sure ack is sent as false
        _dartsNodeMailbox.SUtoNM = *(loc);
    } else return(-1); //not acked

}

void e_darts_set_ack(bool ack)
{
    _dartsNodeMailbox.NMtoSU.ack = ack;
}

bool e_darts_get_ack()
{
    return(_dartsNodeMailbox.NMtoSU.ack);
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

