#include "common.h"

/*
 * Temporal function
 *
 */ 

void sum();

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
void end_rt()
{
    unsigned *cuDone,*suDone;
    cuDone = (unsigned *) CU_DONE_ADDR;
    *cuDone = 1;
    suDone = (unsigned *) (*((unsigned *)CU_SU_BASE_ADDR) + SU_CU_DONE+ 4*MY_CU_NUM);
    *suDone = 1;
    
}

/*
 * At the begining there is a runtime initialization phase.
 * This function initializes the states for the CUs
 *
 */
void init_CU_states(CU_states_t** states)
{
    *states = (CU_states_t *) CU_STATES_BASE_ADDR;
    (*states)->suBaseAddr = (unsigned) e_get_global_address( SU_ROW , SU_COL , 0x0000 );
    // e_group_config and e_emem_config are structures defined
    // in e-lib that contain core values.
    initCodeletsQueue(&((*states)->codeletQueue), 100, (unsigned *)(CU_MY_GLOBAL_BASE_ADDR + CU_QUEUEHEAD_ADDR));
}

/*
 * At the begining there is a runtime initialization phase.
 * This function initializes the states for the SUs
 *
 */
void init_SU_states(SU_states_t ** states)
{
    int i;
    *states = (SU_states_t *) SU_STATES_BASE;
    (*states)->done = 0;
    (*states)->suBaseAddress = (unsigned) e_get_global_address( SU_ROW , SU_COL , 0x0000 );
    for ( i = 0; i < NUM_CU; ++i )
    {
        (*states)->cuBaseAddress[i] = (unsigned) e_get_global_address( (i+1) / 4 , (i+1) % 4  , 0x0000 );
        (*states)->cuDone[i] = 0;
    }
}

/*
 * This is the SU runtime.
 * For now it is in charge of pushing codelets to the CU
 * queues for execution. Then send the dermination codelet
 * and finally wait for them to finish
 *
 */
void _SU_rt()
{

    //Runtime variables
    init_SU_states(&(_rt_states.SU_states));
    
    int i,j;
   
    //for global memory of the CUs
    codeletsQueue_t * codeletQueue;
    unsigned cuCodeletQueueAddr;

    sum();
    
    //Wait for host
    while((_rt_states.SU_states)->startSignal != 0);

    //Wait for CUs to be ready
    for (i = 0; i < NUM_CU; ++i)
    {
        while ((_rt_states.SU_states)->signal[i] != 0);
    }
    
    for (i = 0; i < NUM_CU; ++i)
    {
        cuCodeletQueueAddr = ((unsigned)((_rt_states.SU_states)->cuBaseAddress[i] + CU_CODQUEUE_ADDR));
        codeletQueue = (codeletsQueue_t *) (cuCodeletQueueAddr);
        for ( j = 0; j < 1; ++j )
        {
            //while ( pushCodeletQueue(codeletQueue, (unsigned) ( (_rt_states.SU_states)->suBaseAddress + ((unsigned) &sum))) == 1);
            while ( pushCodeletQueue(codeletQueue, (unsigned) &sum) == 1);
        }
        //Send final codelet
        //while ( pushCodeletQueue(codeletQueue, (unsigned) ( (_rt_states.SU_states)->suBaseAddress + ((unsigned) &end_rt))) == 1);
            while ( pushCodeletQueue(codeletQueue, (unsigned) &end_rt) == 1);
        

    }
    
    for (i = 0; i < NUM_CU; i++)
    {
        while ( (_rt_states.SU_states)->cuDone[i] == 0); 
    }
    
    (_rt_states.SU_states)->done = 1;

}

/*
 * This is the CU runtime.
 * For now it is in charge of checking if we have a codelet
 * in que codelet queue, if so, pop it and execute it.
 * Whenever the termination codelet ends, this method returns
 * and the runtime execution is over
 *
 */
void _CU_rt()
{
    //Runtime variables
    init_CU_states(&(_rt_states.CU_states));
    
    unsigned *value;
    unsigned popedCodelet; 

    codelet toExecute;
     
    value = (unsigned *) 0x2228;
    
    unsigned suAddress;
    suAddress = (unsigned)  e_get_global_address( SU_ROW , SU_COL , 0x0000 );
    
    //Init the queue
    (_rt_states.CU_states)->done = 0;
    *value = 0;
    
    // RT Barrier
    while((_rt_states.CU_states)->startSignal != 0);
    
    //signal SU
    unsigned * su_signal;
    su_signal = (unsigned *) (suAddress + SU_CU_SIGNALS + MY_CU_NUM*sizeof(unsigned));
    *su_signal = 0;
    // This happens forever until the runtime is stopped
    while( (_rt_states.CU_states)->done == 0 || queueEmpty(&(_rt_states.CU_states)->codeletQueue)  != 0)
    {
            //flag to check if there is work to do.
            if ( popCodeletQueue(&((_rt_states.CU_states)->codeletQueue), &popedCodelet) == 0 )
            {
               toExecute = (codelet) popedCodelet;
               toExecute();
            }
    }

}
