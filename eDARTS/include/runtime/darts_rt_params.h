#define _DARTS_NUM_CORES 16
#define _DARTS_NUM_SYNC_SLOTS_PER_CU 10
#define _DARTS_NUM_SYNC_SLOTS_PER_SU 10
#define _DARTS_CODELET_QUEUE_SIZE_BYTES 216 //should be multiple of 12, since all codelets are 12 bytes
#define _DARTS_TPCLOSURES_QUEUE_SIZE_BYTES 500
#define _DARTS_TP_SLOTS_PER_SU 100
#define _DARTS_TP_HEAP_SIZE_BYTES 500
#define _DARTS_MAILBOX_MSG_SIZE 0x7f // 127 for alignment: needs to match the MAX_PAYLOAD_SIZE on host side
