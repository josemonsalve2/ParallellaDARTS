#include "common.h"

/* 
 * This corresponds to the CU state flags. 
 * Used by the runtime to manage the CU.
 *
 * IMPORTANT: Order must be kept since it is used
 * to pack all the information together. Otherwhise
 * take a look at common.h to see what the order is
 * and to apply changes
 */
typedef struct PACKED CU_states_s
{
    unsigned startSignal;
    unsigned done;
    unsigned suBaseAddr;
    codeletsQueue_t codeletQueue;
} CU_states_t;

/* 
 * This corresponds to the SU state flags. 
 * Used by the runtime to manage the SU.
 *
 * IMPORTANT: Order must be kept since it is used
 * to pack all the information together. Otherwhise
 * take a look at common.h to see what the order is
 * and to apply changes
 */
typedef struct PACKED SU_states_s
{
    unsigned startSignal;
    unsigned done;
    unsigned suBaseAddress;
    unsigned signal[NUM_CU];
    unsigned cuBaseAddress[NUM_CU];
    unsigned cuDone[NUM_CU];
} SU_states_t;

/*
 * This union is to avoid using two memory locations for the
 * states that differ on the SU and the CUs
 *
 */
union rt_states {
    SU_states_t *SU_states;
    CU_states_t *CU_states;
} _rt_states;

/*
 * At the end of execution the SU send an end_rt codelet 
 * which executes immediatly and sets the termination flag
 * to 1. This way the CU will return from the runtime and
 * end execution
 */
void end_rt();

/*
 * At the begining there is a runtime initialization phase.
 * This function initializes the states for the CUs
 *
 */
void init_CU_states(CU_states_t** states);

/*
 * At the begining there is a runtime initialization phase.
 * This function initializes the states for the SUs
 *
 */
void init_SU_states(SU_states_t ** states);

/*
 * This is the SU runtime.
 * For now it is in charge of pushing codelets to the CU
 * queues for execution. Then send the dermination codelet
 * and finally wait for them to finish
 *
 */
void _SU_rt();

/*
 * This is the CU runtime.
 * For now it is in charge of checking if we have a codelet
 * in que codelet queue, if so, pop it and execute it.
 * Whenever the termination codelet ends, this method returns
 * and the runtime execution is over
 *
 */
void _CU_rt();
