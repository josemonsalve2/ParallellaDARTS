#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "syncSlot.h"
#include "codelet.h"
#include "codeletsQueue.h"
#include "e_darts_print.h"

#define SYNC_SLOT_ADDR 0x2000
#define BARRIER1_ADDR 0x3000

typedef void (*codeletFunction)();

// print hello world
void printHelloWorld()
{
	e_darts_print("Hello world codelet...\n");
}

void e_producer()
{

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

}

void e_consumer()
{
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

}

void oneToOneSingleCore()
{
	// one to one, single core. No scheduling. Testing dec dep and reset
	    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
	    	e_darts_print("Testing 1 to 1 codelet...\n");
	    	syncSlot_t testSyncSlot1;
	        codelet_t testCodelet1;

	        initCodelet(&testCodelet1, 0 , &testSyncSlot1, printHelloWorld);
	        initSyncSlot (&testSyncSlot1, 0, 2, 2, testCodelet1, 1);
	        int numDep;
	        while ((numDep = syncSlotDecDep(&testSyncSlot1)) != 0) {
	        	e_darts_print("Dependencies = %d\n", numDep);
	        }
	        e_darts_print("Firing codelet\n");
	        testSyncSlot1.codeletTemplate.fire();
	    }
}

#if 0
void oneToOneTwoCores()
{
	    unsigned syncSlotCoreBaseAddress = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
	    unsigned decDepCodeBaseAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );
	    unsigned * barrier1 = (unsigned *) (syncSlotCoreBaseAddress +  BARRIER1_ADDR);
    	syncSlot_t * testSyncSlot1 = (syncSlot_t *) SYNC_SLOT_ADDR;

	// one to one, two cores. Scheduling
	    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
	    	e_darts_print("Testing 1 to 1 codelet two cores...\n");

	        codelet_t testCodelet1;

	        initCodelet(&testCodelet1, 0 , &testSyncSlot1, printHelloWorld);
	        initSyncSlot (testSyncSlot1, 0, 2, 2, testCodelet1, 1);

	        // Sync
	        *barrier1 = 1;
	        while (barrier1 != 2);
	    } else if (e_group_config.core_row == 0 && e_group_config.core_col == 1) {
	    	while (*barrier1 == 0);
	        int numDep;
	        while ((numDep = syncSlotDecDep(&testSyncSlot1)) != 0) {
	        	e_darts_print("Dependencies = %d\n", numDep);
	        }
	        e_darts_print("Firing codelet\n");
	        testSyncSlot1.codeletTemplate.fire();
	        // restore barrier for next part
	        *barrier1 = 2;
	    } else {
	    	while(*barrier1 != 2);
	    }

}
#endif

int main(void)
{
    oneToOneSingleCore();



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
