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

#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "common.h"


/**
 * @brief codelets queue struct
 * @todo change the codelet to be an actual codelet instead of a unsigned;
 * 
 * This struct contains: A size that represents what is the maximum number of
 * elements that can fit the struct. A curNumElements that tells how many 
 * elements there are. A pointer to the head of the queue. This means that the
 * queue structure and the queue itself might be in different locations. A
 * currentTail which is used to calculate where to insert. Finally, a lockMutex
 * used for any operation involving the queue. Since we want to have many 
 * cores accessing it at the same time possibly. 
 * 
 * @warning changing this struct values may cause the alignment to fail 
 * in the CU states struct
 * 
 */

typedef struct PACKED codeletsQueue_s {
    unsigned * headAddress;
    unsigned int size;
    unsigned int curNumElements;
    unsigned int currentTail;
    unsigned int row;
    unsigned int col;
    e_mutex_t lockMutex; // e_mutex_t is an int.
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
void initCodeletsQueue( codeletsQueue_t * queue, unsigned int newSize, unsigned * newHeadAddress);

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

unsigned pushCodeletQueue (codeletsQueue_t * queue, unsigned newCodelet);

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
unsigned popCodeletQueue (codeletsQueue_t * queue, unsigned * popedCodelet);

/** 
 * @brief is queue empty?
 * 
 * check if the queue is empty
 * 
 * \p queue codelet queue to check
 */

unsigned queueEmpty(codeletsQueue_t * queue);
