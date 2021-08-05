#define _DARTS_NUM_CORES 16
#define _DARTS_NUM_SYNC_SLOTS_PER_CU 10
#define _DARTS_NUM_SYNC_SLOTS_PER_SU 10
#define _DARTS_CODELET_QUEUE_SIZE_BYTES 216      //should be multiple of 12, since all codelets are 12 bytes
#define _DARTS_TPCLOSURES_QUEUE_SIZE_BYTES 500
#define _DARTS_TP_SLOTS_PER_SU 100
#define _DARTS_TP_HEAP_SIZE_BYTES 320            //TP metadata & syncSlots in local; 4000 is enough for fib(8)
#define _DARTS_TP_DATA_HEAP_SIZE_BYTES 240       //TP data space in local
#define _DARTS_TP_DATA_HEAP_DRAM_SIZE_BYTES 4000 //TP data space in DRAM
#define _DARTS_MAILBOX_MSG_SIZE 0x7f             //127 for alignment: needs to match the MAX_PAYLOAD_SIZE on host side
#define _DARTS_COMM_QUEUE_LENGTH 8		 // array length of mailboxes for comm queue