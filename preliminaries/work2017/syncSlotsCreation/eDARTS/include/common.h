#define N     4096
#define CORES 16

//For CU states
#define NUM_CU 15
#define CU_MY_ROW e_group_config.core_row
#define CU_MY_COL e_group_config.core_col
#define MY_CU_NUM ((CU_MY_ROW * 4) + CU_MY_COL - 1)
#define CU_MY_GLOBAL_BASE_ADDR e_emem_config.base
#define CU_STATES_BASE_ADDR 0x3000
#define CU_START_SIGNAL CU_STATES_BASE_ADDR
#define CU_DONE_ADDR (CU_STATES_BASE_ADDR + 0x4)
#define CU_SU_BASE_ADDR (CU_STATES_BASE_ADDR + 0x8)
#define CU_CODQUEUE_ADDR (CU_STATES_BASE_ADDR + 0xC)
#define CU_QUEUEHEAD_ADDR (CU_STATES_BASE_ADDR + 0x28)

//For SU states
#define SU_ROW 0
#define SU_COL 0
#define SU_STATES_BASE 0x3000
#define SU_START_SIGNAL SU_STATES_BASE
#define SU_DONE_ADDR (SU_STATES_BASE + 0x4)
#define SU_BASE_ADDR (SU_STATES_BASE + 0x8)
#define SU_CU_SIGNALS (SU_STATES_BASE + 0xC)
#define SU_CU_BASE_ADDR (SU_STATES_BASE + 0xC + 0x4*NUM_CU)
#define SU_CU_DONE (SU_STATES_BASE + 0xC + 0x4*NUM_CU*2)


//RUNtIME:
#define RT_START_SIGNAL 0x3000

//Experiments
#define COUNT_TO 1000

typedef void (*codelet)();
typedef int  darts_mutex_t;
typedef DARTS_MUTEX_NULL;
