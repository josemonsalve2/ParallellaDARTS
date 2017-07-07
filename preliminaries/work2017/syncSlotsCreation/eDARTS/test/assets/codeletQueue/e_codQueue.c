//First implementation of a codeletsQueue for the parallella

#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "codeletsQueue.h"

#define GLOBAL_BASE_ADDR e_emem_config.base
#define SUM_RESULT 0x2228
#define STATES_BASE_ADDR 0x3000  /// Chosen randomly
#define START_SIGNAL CU_STATES_BASE_ADDR // consumer->producer(ready to receieve)
#define DONE_SIGNAL CU_STATES_BASE_ADDR // producer->consumer (done pushing codelets)
#define CODQUEUE_ADDR (CU_STATES_BASE_ADDR + 0x4) // Metadata and handler
#define QUEUEHEAD_ADDR (CU_STATES_BASE_ADDR + 0x20) // Actual queue sizeMetadata=0x1C

typedef void (*codelet)();

//function to execute
void sum() __attribute__((section (".internaltext")));
void sum()
{
    unsigned *value;
    value = (unsigned *) SUM_RESULT;

    (*value) ++;
}

void e_producer()
{
    
    int i=0; // for loop interations

    // For sending and receiving address
    unsigned producerBaseAddress = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
    unsigned consumerBaseAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );
    
    // The codelet queue
    unsigned consumerCodeletQueueAddr = consumerBaseAddress + CODQUEUE_ADDR;
    codeletsQueue_t * codeletQueue = (codeletsQueue_t *) consumerCodeletQueueAddr;
    
    // flags
    unsigned *doneSig = (unsigned *)(consumerBaseAddress + DONE_SIGNAL);
    unsigned *startSig = (unsigned *)(producerBaseAddress + START_SIGNAL);

    while (*startSig == 0);
    
    //Obtain the sum address (considering sum function points to the plt entry)
    unsigned pltEntryAddr = (unsigned)sum;
    unsigned sumPtr = producerBaseAddress + *((unsigned *)pltEntryAddr + 2); // 2 for the actual address
    
    for (i = 0 ; i < 100000; i++) {
        while ( pushCodeletQueue(codeletQueue, sumPtr) == 1);
    }

     *doneSig=1;

}

void e_consumer()
{
    // For sending and receiving address
    unsigned producerBaseAddress = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
    unsigned consumerBaseAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );

    // The codelet queue
    unsigned consumerCodeletQueueAddr = consumerBaseAddress + CODQUEUE_ADDR;
    unsigned codeletQueueHead = consumerBaseAddress + QUEUEHEAD_ADDR;
    codeletsQueue_t * codeletQueue = (codeletsQueue_t *) consumerCodeletQueueAddr;

    // to fetch from queue
    unsigned codeletAddr;
    codelet toExecute;
    
    // flags
    unsigned *doneSig = (unsigned *) (consumerBaseAddress + DONE_SIGNAL);
    unsigned *startSig = (unsigned *) (producerBaseAddress + START_SIGNAL);
    
    // sumResult initialization
    unsigned *sumResult = (unsigned *)SUM_RESULT;
    *sumResult = 0;
    
     //Init the queue (pointer,size, headPtr)
    initCodeletsQueue(codeletQueue, 300, (unsigned*)codeletQueueHead);
    
    *startSig = 1;
    while(*doneSig == 0 || queueEmpty(codeletQueue) != 0)
    {        
        if ( popCodeletQueue(codeletQueue, &codeletAddr) == 0 ) // Assuming core(0,0)
        {
            toExecute = (codelet) codeletAddr;;
            toExecute();
        }
    }
    *doneSig = 2;
    
}

int main(void)
{
    // core (0,0) is producer
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0)
    {
        e_producer();
    }
    else // core (0,1) is consumer
    {
        e_consumer();
    }
        
    return 0;
} 