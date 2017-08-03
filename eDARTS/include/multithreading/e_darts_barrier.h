/**
 * @file e_darts_barrier.h
 * @author Jose M Monsalve
 * @date 10 July 2017
 * @brief Barrier implementation for eDARTS
 * @todo Add copyright
 *
 * Barriers allows to have multiple threads reach the same spot in code
 * and wait for each other
 *
 *
 */
#ifndef E_DARTS_BARRIER_H
#define E_DARTS_BARRIER_H

#include "e_darts_mutex.h"
#include "codelet.h"

#define __MAX_NUM_THREADS_BARRIER 16

/**
 * @brief barrier Struct.
 *
 * This barrier does an atomic add on a counter, if the counter
 * reaches the numThreads, then the control is released.
 *
 * @TODO think of a possible implementation with WAND instead of this way
 *
 * if __MAX_NUM_THREADS_BARRIER = 16
 *    Size of barrier = 4 + 4 + 4 + 4 + 4*16 = 80
 */
typedef struct __attribute__((__packed__)) darts_barrier_s {
	darts_mutex_t lock;
	darts_mutex_t * lockFullAddressPtr;
	volatile unsigned counter;
	unsigned numThreads;
	unsigned mailboxes[__MAX_NUM_THREADS_BARRIER];
} darts_barrier_t;

/**
 * @brief (not thread safe) creates a barrier and initializes the value.
 *
 * The number of threads is used for the threads to wait until it reaches
 * that value.
 *
 * @TODO think of a possible implementation with WAND instead of this way
 *
 */
void darts_barrier_init( volatile darts_barrier_t * newBarrier, unsigned numThreads);

/**
 * @brief makes the thread to wait until the counter reaches the number of threads
 *
 * This function creates a new variable that is used as mailbox for notification
 * when the barrier is reached. This variable is in the local core, and so it is
 * possible to spin lock without going to the NoC and it won't cause new calls to
 * this barrier to be a problem
 *
 * @TODO think of a possible implementation with WAND instead of this way
 *
 */
unsigned darts_barrier(volatile darts_barrier_t * barrier);


#endif /* E_DARTS_BARRIER_H */
