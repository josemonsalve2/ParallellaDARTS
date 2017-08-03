//First implementation of a codeletsQueue for the parallella

#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "codeletsQueue.h"
#include "codelet.h"

#define SUM_RESULT 0x2228
#define STATES_BASE_ADDR 0x3000  /// Chosen randomly
#define START_SIGNAL STATES_BASE_ADDR // consumer->producer(ready to receieve)
#define DONE_SIGNAL STATES_BASE_ADDR // producer->consumer (done pushing codelets)
#define CODQUEUE_ADDR (STATES_BASE_ADDR + 0x4) // Metadata and handler

typedef void (*codeletFunction)();

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
        codelet_t codelet;
        codelet.fire = (codeletFunction) sumPtr;
        while(pushCodeletQueue(codeletQueue, &codelet) != CODELET_QUEUE_SUCCESS_OP);
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
    codeletsQueue_t * codeletQueue = (codeletsQueue_t *) consumerCodeletQueueAddr;

    // to fetch from queue
    codelet_t codelet;

    // flags
    unsigned *doneSig = (unsigned *) (consumerBaseAddress + DONE_SIGNAL);
    unsigned *startSig = (unsigned *) (producerBaseAddress + START_SIGNAL);

    // sumResult initialization
    unsigned *sumResult = (unsigned *)SUM_RESULT;
    *sumResult = 0;

     //Init the queue (pointer,size, headPtr)
    initCodeletsQueue(codeletQueue, 300);

    *startSig = 1;
    while(*doneSig == 0 || !queueEmpty(codeletQueue)) {
        if (popCodeletQueue(codeletQueue, &codelet) == CODELET_QUEUE_SUCCESS_OP ) // Assuming core(0,0)
        {
            codelet.fire();
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
