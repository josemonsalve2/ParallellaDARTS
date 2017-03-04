#define N     4096
#define CORES 16
#define SU_ROW  0
#define SU_COL  0

//For CU states
#define CU_ROW e_group_config.core_row
#define CU_COL e_group_config.core_col
#define CU_BASE_ADDR e_emem_config.base
#define CU_STATES_BASE_ADDR 0x2000
#define CU_DONE_ADDR CU_STATES_BASE_ADDR
#define CU_CODQUEUE_ADDR (CU_STATES_BASE_ADDR + 0x4)
#define CU_QUEUEHEAD_ADDR (CU_STATES_BASE_ADDR + 0x18)

//For SU states
#define SU_ROW e_group_config.core_row
#define SU_COL e_group_config.core_col



typedef void (*codelet)();
