#define N     4096
#define CORES 16

//For CU states
#define NUM_CU 1
#define CU_MY_ROW e_group_config.core_row
#define CU_MY_COL e_group_config.core_col
#define MY_CU_NUM ((CU_MY_ROW * 4) + CU_MY_COL - 1)
#define CU_BASE_ADDR e_emem_config.base
#define CU_STATES_BASE_ADDR 0x2004
#define CU_DONE_ADDR CU_STATES_BASE_ADDR
#define CU_CODQUEUE_ADDR (CU_STATES_BASE_ADDR + 0x4)
#define CU_QUEUEHEAD_ADDR (CU_STATES_BASE_ADDR + 0x18)

//For SU states
#define SU_ROW 0
#define SU_COL 0
#define SU_STATES_BASE 0x2004

//RUNtIME:
#define RT_START_SIGNAL 0x2000



typedef void (*codelet)();
