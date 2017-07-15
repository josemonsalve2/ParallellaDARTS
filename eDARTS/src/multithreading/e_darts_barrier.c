#include "e_darts_barrier.h"
#include "e_darts_print.h"
void darts_barrier_init( volatile darts_barrier_t * newBarrier, unsigned numThreads) {
	int i;
	newBarrier->lock = DARTS_MUTEX_NULL;
	unsigned coreID;
	__asm__ __volatile__ ("movfs %0, coreid" : "=r" (coreID));
	newBarrier->lockFullAddressPtr = (darts_mutex_t *) ((coreID << 20) + (unsigned)(((unsigned)&newBarrier->lock) & 0x000FFFFF));
	newBarrier->counter = 0;
	newBarrier->numThreads = numThreads;
	for (i = 0; i < numThreads; i++) {
		newBarrier->mailboxes[i] = 0;
	}
}

unsigned _private_darts_barrier(volatile darts_barrier_t * barrier, volatile unsigned * mailboxVar) {
	// mailboxVar must be zero and address must be complete
	if (*mailboxVar != 0 || !((unsigned)mailboxVar & 0x000FFFFF))
		return 1; // Returns 1 if there was an error

	int i;
	darts_mutex_lock(barrier->lockFullAddressPtr);
	if (barrier->counter == 0) {
		// If you are the first initialize all the mailboxes, place
		// your mailbox variable and wait
		for (i = 0 ; i < barrier->numThreads; i++) {
			barrier->mailboxes[i] = 0;
		}
	} else if (barrier->counter == barrier->numThreads - 1) {
		// if you are the last, send a message to all the other mailboxes
		for (i = 0; i < barrier->numThreads; i++) {
			unsigned * toSignal = (unsigned *) barrier->mailboxes[i];
			*toSignal = 1; // Signal the barrier
		}
		// Reset the counter for the next barrier to take place
		barrier->counter = 0;
		darts_mutex_unlock(barrier->lockFullAddressPtr);
		return 0;
	}
	// Register your mailbox address and increment the counter
	barrier->mailboxes[barrier->counter] = (unsigned) mailboxVar;
	barrier->counter++;
	darts_mutex_unlock(barrier->lockFullAddressPtr);
	while (*mailboxVar == 0);
	return 0;
}

unsigned darts_barrier(volatile darts_barrier_t * barrier) {
	unsigned mailboxVar = 0;
	unsigned coreID;
	unsigned * mailboxVarFullAddress;
	// We make sure the address is complete
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (coreID));
    mailboxVarFullAddress = (unsigned *) ((coreID << 20) + (unsigned)(((unsigned)&mailboxVar) & 0x000FFFFF));

    // We call the actual barrier block
    return _private_darts_barrier(barrier, mailboxVarFullAddress);
}
#undef _private_darts_barrier // Make sure nobody calls this function
