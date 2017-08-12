#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "syncSlot.h"
#include "codelet.h"
#include "e_darts_print.h"
#include "e_darts_barrier.h"

#define INIT_FLAG 0x2000
#define FINI_FLAG 0x2004
#define SYNC_SLOT_ADDR 0x2008
#define BARRIER_ADDR 0x2030

typedef void (*codeletFunction)();

darts_barrier_t * barrierAll;

// print hello world
void printHelloWorld()
{
	e_darts_print("\x1B[31mHello world codelet...\x1B[0m\n");
}

void initialization()
{
    unsigned base0_0address = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
	unsigned * initFlag = (unsigned *) (base0_0address + INIT_FLAG);
	barrierAll = (darts_barrier_t *) (base0_0address + BARRIER_ADDR);

    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
    	darts_barrier_init(barrierAll,16);
    	*initFlag = 1;
    	e_darts_print("Finishing initialization \n");
    } else {
    	while (*initFlag != 1);
    }
}

void finalize() {
    unsigned base0_0address = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
	unsigned * finiFlag = (unsigned *) (base0_0address + FINI_FLAG);

    if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
    	*finiFlag = 1;
    	e_darts_print("Finishing execution \n");
    }
    darts_barrier(barrierAll);
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
	    darts_barrier(barrierAll);
}

void oneToOneTwoCores()
{
	unsigned syncSlotCoreBaseAddress = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
	unsigned decDepCodeBaseAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );
	syncSlot_t * testSyncSlot1 = (syncSlot_t *) (syncSlotCoreBaseAddress +SYNC_SLOT_ADDR);

	// one to one, two cores. basic scheduling
	if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
		e_darts_print("Testing 1 to 1 codelet two cores...\n");

		codelet_t testCodelet1;

		initCodelet(&testCodelet1, 0 , testSyncSlot1, printHelloWorld);
		initSyncSlot(testSyncSlot1, 0, 2, 2, testCodelet1, 1);
		darts_barrier(barrierAll);

	} else if (e_group_config.core_row == 0 && e_group_config.core_col == 1) {

		darts_barrier(barrierAll);
		int numDep;
		while ((numDep = syncSlotDecDep(testSyncSlot1)) != 0) {
			//e_darts_print("Dependencies = %d\n", numDep);
		}
		e_darts_print("Firing codelet\n");
		testSyncSlot1->codeletTemplate.fire();
		// restore barrier for next part

	} else {
		darts_barrier(barrierAll);
	}
	darts_barrier(barrierAll);

}

void oneToOneTwoCoresManyCodelets()
{
	unsigned syncSlotCoreBaseAddress = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
	unsigned decDepCodeBaseAddress = (unsigned) e_get_global_address( 0 , 1 , 0x0000 );
	syncSlot_t * testSyncSlot1 = (syncSlot_t *) (syncSlotCoreBaseAddress +SYNC_SLOT_ADDR);

	// one to one, two cores. 15 codelets Scheduling
	if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
		e_darts_print("Testing 1 core to 1 core, single syncSlot, 5 codelets spawned...\n");

		codelet_t testCodelet1;

		initCodelet(&testCodelet1, 0 , testSyncSlot1, printHelloWorld);
		initSyncSlot(testSyncSlot1, 0, 2, 2, testCodelet1, 5);
		darts_barrier(barrierAll);

	} else if (e_group_config.core_row == 0 && e_group_config.core_col == 1) {

		darts_barrier(barrierAll);
		int numDep;
		while ((numDep = syncSlotDecDep(testSyncSlot1)) != 0) {
			//e_darts_print("Dependencies = %d\n", numDep);
		}
		e_darts_print("Firing codelet\n");
		int i;
		for (i = 0; i < testSyncSlot1->numCodelets; i++)
			testSyncSlot1->codeletTemplate.fire();
		// restore barrier for next part

	} else {
		darts_barrier(barrierAll);
	}
	darts_barrier(barrierAll);
}
void oneToManyMultipleCores()
{
	int i;
	unsigned syncSlotCoreBaseAddress = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
	syncSlot_t * testSyncSlot1 = (syncSlot_t *) (SYNC_SLOT_ADDR);

	// one to many, 16 cores. each core spawns two codelets Scheduling
	if (!(e_group_config.core_row == 0 && e_group_config.core_col == 0)) {
		codelet_t testCodelet1;
		initCodelet(&testCodelet1, 0 , testSyncSlot1, printHelloWorld);
		initSyncSlot(testSyncSlot1, 0, 2, 2, testCodelet1, 2);
		darts_barrier(barrierAll);
		while ((testSyncSlot1->currentDep) != 0);
		e_darts_print("Firing codelets from core %d - %d\n", e_group_config.core_row, e_group_config.core_col);

		for (i = 0; i < testSyncSlot1->numCodelets; i++)
			testSyncSlot1->codeletTemplate.fire();

	} else {
		//Only core 0,0
		e_darts_print("Testing 1 core to 15 syncSlots in different cores...\n");
		darts_barrier(barrierAll);

		for (i = 1; i < 16 ; i++){
			unsigned decDepCodeBaseAddress = (unsigned) e_get_global_address( i/4 , i % 4 , 0x0000 );
			testSyncSlot1 = (syncSlot_t *) (SYNC_SLOT_ADDR + decDepCodeBaseAddress);
			syncSlotDecDep(testSyncSlot1);
			syncSlotDecDep(testSyncSlot1);
		}
	}
	darts_barrier(barrierAll);
}

void manyToOneMultipleCores()
{
	int i;
	unsigned syncSlotCoreBaseAddress = (unsigned) e_get_global_address( 0 , 0 , 0x0000 );
	syncSlot_t * testSyncSlot1 = (syncSlot_t *) (syncSlotCoreBaseAddress + SYNC_SLOT_ADDR);

	// one to one, two cores. Scheduling
	if (e_group_config.core_row == 0 && e_group_config.core_col == 0) {
		e_darts_print("Testing many cores to 1 syncSlot with 2 codelet...\n");
		// Initialize slot
		codelet_t testCodelet1;
		initCodelet(&testCodelet1, 0 , testSyncSlot1, printHelloWorld);
		initSyncSlot(testSyncSlot1, 0, 30, 30, testCodelet1, 2);
		darts_barrier(barrierAll);
		while ((testSyncSlot1->currentDep) != 0);
		e_darts_print("Firing codelets from core %d - %d\n", e_group_config.core_row, e_group_config.core_col);

		for (i = 0; i < testSyncSlot1->numCodelets; i++)
			testSyncSlot1->codeletTemplate.fire();

	} else {
		//Other cores
		darts_barrier(barrierAll);

		//Dec dependencies
		syncSlotDecDep(testSyncSlot1);
		syncSlotDecDep(testSyncSlot1);
	}
	darts_barrier(barrierAll);
}



int main(void)
{

	initialization();

    oneToOneSingleCore();
    oneToOneTwoCores();
    oneToOneTwoCoresManyCodelets();
    oneToManyMultipleCores();
    manyToOneMultipleCores();

    finalize();

    return 0;
}
