#include "common.h"
#include "e_DARTS.h"

void end_rt()
{
    unsigned *cuDone,*suDone;
    cuDone = (unsigned *) CU_DONE_ADDR;
    *cuDone = 1;
    suDone = (unsigned *) (*((unsigned *)CU_SU_BASE_ADDR) + SU_CU_DONE+ 4*MY_CU_NUM);
    *suDone = 1;
    
}

void init_CU_states(CU_states_t** states)
{
    *states = (CU_states_t *) CU_STATES_BASE_ADDR;
    (*states)->suBaseAddr = (unsigned) e_get_global_address( SU_ROW , SU_COL , 0x0000 );
    // e_group_config and e_emem_config are structures defined
    // in e-lib that contain core values.
    initCodeletsQueue(&((*states)->codeletQueue), 100, (unsigned *)(CU_MY_GLOBAL_BASE_ADDR + CU_QUEUEHEAD_ADDR));
}

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

void _SU_rt()
{

    //Runtime variables
    init_SU_states(&(_rt_states.SU_states));
    
    int i,j;
   
    //for global memory of the CUs
    codeletsQueue_t * codeletQueue;
    unsigned cuCodeletQueueAddr;

    unsigned *cd;
    cd = 0x4000;
    codelet sum;
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
            while ( pushCodeletQueue(codeletQueue, cd) == 1);
        }
        //Send final codelet
            while ( pushCodeletQueue(codeletQueue, (unsigned) &end_rt) == 1);
        

    }
    
    for (i = 0; i < NUM_CU; i++)
    {
        while ( (_rt_states.SU_states)->cuDone[i] == 0); 
    }
    
    (_rt_states.SU_states)->done = 1;

}

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
    while((_rt_states.CU_states)->done == 0 || queueEmpty(&(_rt_states.CU_states)->codeletQueue)  != 0)
    {
            //flag to check if there is work to do.
            if ( popCodeletQueue(&((_rt_states.CU_states)->codeletQueue), &popedCodelet) == 0 )
            {
               toExecute = (codelet) popedCodelet;
               toExecute();
            }
    }
}
