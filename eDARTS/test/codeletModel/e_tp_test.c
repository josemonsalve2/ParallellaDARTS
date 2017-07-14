//First implementation of a codeletsQueue for the parallella

#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "codelet.h"
#include "e_tp_definition_test.h"
#include "e_darts_barrier.h"

#define INIT_SIGNAL 0x2004
#define FINAL_SIGNAL 0x2008
#define BARRIER_LOCATION 0x200C
#define TP_CLOSURE_LOCATION 0x202A
#define TP_LOCATION 0x202A
#define LOCAL_MEM_LOCATION 0x3000
#define EXTERNAL_MEM_LOCATION 0x8f000000




typedef void (*codeletFunction)();

//function to execute
void sum() __attribute__((section (".internaltext")));
void sum()
{
    unsigned *value;
    value = (unsigned *) SUM_RESULT;

    (*value) ++;
}

void e_tpDefiner(darts_barrier_t * threadsBarrier) {

}

void e_tpCreatorExecuter(darts_barrier_t * threadsBarrier) {

}


//
//void e_producer()
//{
//
//    int i=0; // for loop interations
//
//    // For sending and receiving address
//    unsigned producerBaseAddress = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
//    unsigned consumerBaseAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );
//
//    // The codelet queue
//    unsigned consumerCodeletQueueAddr = consumerBaseAddress + CODQUEUE_ADDR;
//    codeletsQueue_t * codeletQueue = (codeletsQueue_t *) consumerCodeletQueueAddr;
//
//    // flags
//    unsigned *doneSig = (unsigned *)(consumerBaseAddress + DONE_SIGNAL);
//    unsigned *startSig = (unsigned *)(producerBaseAddress + START_SIGNAL);
//
//    while (*startSig == 0);
//
//    //Obtain the sum address (considering sum function points to the plt entry)
//    unsigned pltEntryAddr = (unsigned)sum;
//    unsigned sumPtr = producerBaseAddress + *((unsigned *)pltEntryAddr + 2); // 2 for the actual address
//
//    for (i = 0 ; i < 100000; i++) {
//        codelet_t codelet;
//        codelet.fire = (codeletFunction) sumPtr;
//        while ( pushCodeletQueue(codeletQueue, codelet) == 1);
//    }
//
//     *doneSig=1;
//
//}
//
//void e_consumer()
//{
//    // For sending and receiving address
//    unsigned producerBaseAddress = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
//    unsigned consumerBaseAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );
//
//    // The codelet queue
//    unsigned consumerCodeletQueueAddr = consumerBaseAddress + CODQUEUE_ADDR;
//    unsigned codeletQueueHead = consumerBaseAddress + QUEUEHEAD_ADDR;
//    codeletsQueue_t * codeletQueue = (codeletsQueue_t *) consumerCodeletQueueAddr;
//
//    // to fetch from queue
//    codelet_t codelet;
//
//    // flags
//    unsigned *doneSig = (unsigned *) (consumerBaseAddress + DONE_SIGNAL);
//    unsigned *startSig = (unsigned *) (producerBaseAddress + START_SIGNAL);
//
//    // sumResult initialization
//    unsigned *sumResult = (unsigned *)SUM_RESULT;
//    *sumResult = 0;
//
//     //Init the queue (pointer,size, headPtr)
//    initCodeletsQueue(codeletQueue, 300, (unsigned*)codeletQueueHead);
//
//    *startSig = 1;
//    while(*doneSig == 0 || queueEmpty(codeletQueue) != 0)
//    {
//        if ( popCodeletQueue(codeletQueue, &codelet) == 0 ) // Assuming core(0,0)
//        {
//            codelet.fire();
//        }
//    }
//    *doneSig = 2;
//
//}

int main(void)
{

    unsigned base0_0Address = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
    unsigned *initSignal = (unsigned *) (base0_0Address + INIT_SIGNAL);
    unsigned *finalSignal = (unsigned *) (base0_0Address + FINAL_SIGNAL);
    darts_barrier_t * myBarrier =(darts_barrier_t *) ( base0_0Address + BARRIER_LOCATION);

    // core (0,0) is producer
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0)
    {
        darts_barrier_init(myBarrier,16);
        *initSignal = 1;
        e_darts_print("\n === TP TESTING ===\n");
        e_tpDefiner(myBarrier);
    }
    else if (e_group_config.core_row == 0 && e_group_config.core_col == 1)// core (0,1) is consumer
    {
        while (*initSignal == 0);
        e_tpCreatorExecuter(myBarrier);
    }

    darts_barrier(myBarrier);
    if (e_group_config.core_row == 0 && e_group_config.core_col == 0)
    {
        e_darts_print("\n === END TP TESTING: ===\n");
        *finalSignal = 1;
    }


    return 0;
}
