/**
 * @file codeletsQueue.h
 * @author Jose M Monsalve
 * @date 2 March 2017
 * @brief File defining the codeletsQueue_t parallel struct.
 * @todo Add copyright
 *
 * codeletsQueue_t is an structure that allows a queue to be accessed
 * concurrently. There is a mutex that locks the queue on push or pop
 * operations. operations over a queue are defined in functions.
 */

#ifndef CODELETSQUEUE_H
#define CODELETSQUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "e_darts_mutex.h"
#include "codelet.h"

// Return codes
#define CODELET_QUEUE_SUCCESS_OP 0
#define CODELET_QUEUE_INVALID_QUEUE_ADDRESS 1
#define CODELET_QUEUE_NOT_ENOUGH_SPACE 2
#define CODELET_QUEUE_EMPTY_QUEUE 3

/**
 * @brief codelets queue struct
 *
 *  This struct represents the metadata for the codeletsQueue. The metadata
 *  contains the information of the queue itself. This information is
 *  the head and tail pointers, the total reserved size of the queue, and the
 *  lock for insertion and deletion.
 *
 *  The number of elements should be given by the current used space divided by
 *  the size of a codelet_t.
 *
 *  In memory we expect the queue to look like this
 *
 *   Memory organization of a tpClosuresQueue
 *
 * /--------------------------------\
 * | void * headAddress;            |
 * | void * tailAddress;            |
 * | unsigned size;                 |
 * | darts_mutex_t lockMutex;       |
 * |--------------------------------|
 * |         QUEUE RESERVED         |<-- initPosition = tpClosureQueueAddress + 4*4
 * |             SPACE              |
 * |                                |
 * |********************************|<-- headAddress
 * |********************************|
 * |********************************|
 * |*********QUEUE CONTENT**********|
 * |********************************|
 * |********************************|
 * |********************************|
 * |********************************|
 * |********************************|<-- tailAddress
 * |                                |
 * |         QUEUE RESERVED         |
 * |             SPACE              |
 * |                                |<-- initPosition + size;
 * \--------------------------------/
 *
 */

typedef struct __attribute__ ((__packed__)) codeletsQueue_s {
    void * headAddress;
    void * tailAddress;
    unsigned size;
    darts_mutex_t lockMutex; // e_mutex_t is an int.
    unsigned fullFlag;
} codeletsQueue_t;

/**
 * @brief Function to set all the values to its initial value in the codeletQueue
 * @todo return something?
 * @todo Probably checking the address is not local but global.
 *
 * this function initializes all the elements of the queue. Specially the mutex.
 *
 * It is important to notice that although there is a initMutex function in the
 * eSDK of the parallella, we are not using it because it has a bug. After eSDK 16.3
 * they made this function a no-op. Instead it is necessary to use the MUTEX_NULL
 * paramether
 *
 * @warning @p newHeadAddress must be global if you want to be able to access the
 * queue from other cores
 *
 * @p queue Codelet Queue
 * @p newSize The queue size
 * @p newHeadAddress the address where the actual queue is located
 *
 */
unsigned initCodeletsQueue( codeletsQueue_t * queue, unsigned int newSize);

/**
 * @brief push a codelet from the queue
 * @todo Probably get a codelet instead of a unsigned value
 * @todo make sure the codelet has a global address
 * @todo instead of spinlocking for the mutex, we should define a better mechanism
 *
 * push a codelet to the queue. This function locks the queue, inserts the codelet
 * into the tail and updates all the other necessary values. If the queue is full it will
 * return 1, otherwise, the codelet will be inserted and the function will return 0
 *
 * @p queue Codelet Queue
 * @p newCodelet codelet to insert
 * @return
 */

unsigned pushCodeletQueue (codeletsQueue_t * queue, codelet_t * newCodelet);

/**
 * @brief pop a codelet from the queue
 * @todo Probably get a codelet instead of a unsigned value
 * @todo make sure the codelet has a global address
 * @todo instead of spinlocking for the mutex, we should define a better mechanism
 *
 * Pop a codelet to the queue. This function locks the queue, removes the codelet
 * to the pointer and updates all the other necessary values. If the queue is empty it will
 * return 1 since there is nothing to return, and the pointer will be set up to 0,
 * otherwise, the codelet will be returned in the pointer and the function will return 0
 *
 * @p queue Codelet Queue
 * @p popedCodelet codelet that was taken from the queue
 */
unsigned popCodeletQueue (codeletsQueue_t * queue, codelet_t * popedCodelet);

/**
 * @brief is queue empty?
 *
 * check if the queue is empty
 *
 * \p queue codelet queue to check
 */

unsigned queueEmpty(codeletsQueue_t * queue);

#endif /* CODELETSQUEUE_H */
