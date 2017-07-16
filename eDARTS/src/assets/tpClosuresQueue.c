#include "tpClosuresQueue.h"

// Private function returns the available free space
inline unsigned _calculateMaxFreeSpaceQueue(tpClosuresQueue_t * queue) {
    unsigned initAddress = ((unsigned) queue) + sizeof(tpClosuresQueue_t);
    if ((unsigned) queue->tailAddress >= (unsigned) queue->headAddress ) {
        // No padding included, No wrap of content
        unsigned leftOfHead = ((unsigned)queue->headAddress) - initAddress;
        unsigned rightOfTail = (initAddress + queue->size) - ((unsigned)queue->tailAddress);
        return (rightOfTail >= leftOfHead)? rightOfTail : leftOfHead; // return the max
    } else {
        return ((unsigned) queue->headAddress) - ((unsigned)queue->tailAddress);
    }
    return 0;
}

inline unsigned _checkIfValidAddress (tpClosuresQueue_t * queue) {
    unsigned currentCoreID;
    // We make sure the address is complete
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (currentCoreID));

    // We check if we are getting the whole address.
    if (((unsigned) queue) >> 20 != currentCoreID ) {
        return INVALID_QUEUE_ADDRESS;
    }
    return SUCCESS_OP;
}

unsigned isTpClosureQueueEmpty(tpClosuresQueue_t * queue) {
    return (queue->headAddress == queue->tailAddress);
}

unsigned initTpClosuresQueue(tpClosuresQueue_t * queue, unsigned int newSize) {
    if (_checkIfValidAddress(queue) != SUCCESS_OP) {
        return INVALID_QUEUE_ADDRESS;
    }

    void * init = (void *) (((unsigned) queue) + sizeof(tpClosuresQueue_t));

    //Initialize parameters
    queue->headAddress = init;
    queue->tailAddress = init;
    queue->padding = 0;
    queue->size = newSize;
    queue->lockMutex = DARTS_MUTEX_NULL;

    return SUCCESS_OP;
}

unsigned pushTpClosureQueue (tpClosuresQueue_t * queue, genericTpClosure_t * newTpClosure) {
    if (_checkIfValidAddress(queue) != SUCCESS_OP) {
        return INVALID_QUEUE_ADDRESS;
    }

    darts_mutex_t * mutexPtr = (darts_mutex_t *) (((unsigned)queue) + 2 * sizeof(void *) + 2*sizeof(unsigned));
    // We lock the queue
    darts_mutex_lock(mutexPtr);

    unsigned newTpSize = sizeof(genericTpClosure_t) + newTpClosure->sizeOfArgs;
    unsigned currentMaxFreeSpace = _calculateMaxFreeSpaceQueue(queue);

    // Check if there is space
    if (newTpSize > currentMaxFreeSpace) {
        darts_mutex_unlock(mutexPtr);
        return NOT_ENOUGH_SPACE;
    }

    // We check if head is greater than the tail. Normal insertion. There is padding already
    if ((unsigned) queue->tailAddress < (unsigned) queue->headAddress) {
        memcpy(queue->tailAddress, newTpClosure, newTpSize);
        // Update the tail
        queue->tailAddress = (void *) (((unsigned)queue->tailAddress) + newTpSize);
    } else {
        unsigned initAddress = ((unsigned) queue) + sizeof(tpClosuresQueue_t);
        // check if we need to do padding
        unsigned spaceAfterTail = initAddress + queue->size - ((unsigned) queue->tailAddress);
        if (spaceAfterTail >= newTpSize) {
            // No padding needed
            memcpy(queue->tailAddress, newTpClosure, newTpSize);
            // Update the tail
            queue->tailAddress = (void *) (((unsigned)queue->tailAddress) + newTpSize);
        } else {
            //padding needed padding is used when popping elements
            queue->padding = spaceAfterTail;
            // Insert at the beginning of the space
            void * initPositionQueueSpace = (void *) initAddress;
            memcpy(initPositionQueueSpace, newTpClosure, newTpSize);
            // Tail goes to the end of the newly inserted element
            queue->tailAddress = (void *) (initAddress + newTpSize);
        }
    }

    // We unlock the queue
    // We lock the queue
    darts_mutex_unlock(mutexPtr);
    return SUCCESS_OP;
}

unsigned ownTpClosureQueue (tpClosuresQueue_t * queue) {
    if (_checkIfValidAddress(queue) != SUCCESS_OP) {
        return INVALID_QUEUE_ADDRESS;
    }

    // get the mutex address
    darts_mutex_t * mutexPtr = (darts_mutex_t *) (((unsigned)queue) + 2 * sizeof(void *) + 2*sizeof(unsigned));
    // We lock the queue
    darts_mutex_lock(mutexPtr);
    // Check if it is owned already
    if (queue->queueOwner != 0) {
        // Queue Already Own
        darts_mutex_unlock(mutexPtr);
        return NOT_OWNER;
    }
    // Own queue
    unsigned currentCoreID;
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (currentCoreID));
    queue->queueOwner = currentCoreID;

    darts_mutex_unlock(mutexPtr);
    return SUCCESS_OP;
}

unsigned disownTpClosureQueue (tpClosuresQueue_t * queue) {
    if (_checkIfValidAddress(queue) != SUCCESS_OP) {
        return INVALID_QUEUE_ADDRESS;
    }

    // Since there is only one owner, this operation does not require
    // to lock the queue


    // Only the owner can disown. Check if it is the owner
    unsigned currentCoreID;
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (currentCoreID));
    if (queue->queueOwner != currentCoreID) {
        return NOT_OWNER;
    }

    // Disown queue
    queue->queueOwner = 0;

    return SUCCESS_OP;
}

unsigned peakTopElement(tpClosuresQueue_t * queue, genericTpClosure_t * topElementPointer) {
    if (_checkIfValidAddress(queue) != SUCCESS_OP) {
           return INVALID_QUEUE_ADDRESS;
       }

    // Since there is only one owner, and only owners can peak,
    // this operation does not require to lock the queue

    unsigned currentCoreID;
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (currentCoreID));
    // Only the owner can peak. Check if owner
    if (queue->queueOwner != currentCoreID) {
       topElementPointer = 0;
       return NOT_OWNER;
    }

    // check if queue is empty
    if(isTpClosureQueueEmpty(queue)) {
        topElementPointer = 0;
        return EMPTY_QUEUE;
    }

    // return topElementPointer
    topElementPointer = (genericTpClosure_t *) queue->headAddress;

    return SUCCESS_OP;
}

unsigned popTopElementQueue (tpClosuresQueue_t * queue) {
    if (_checkIfValidAddress(queue) != SUCCESS_OP) {
           return INVALID_QUEUE_ADDRESS;
    }

    // Since there is only one owner, and only owners can peak,
    // this operation does not require to lock the queue

    unsigned currentCoreID;
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (currentCoreID));
    // Only the owner can peak. Check if owner
    if (queue->queueOwner != currentCoreID) {
       return NOT_OWNER;
    }

    // check if queue is empty
    if(isTpClosureQueueEmpty(queue)) {
        return EMPTY_QUEUE;
    }

    unsigned topElementSize = sizeof(genericTpClosure_t) + ((genericTpClosure_t *)queue->headAddress)->sizeOfArgs;

    queue->headAddress = (void *) (((unsigned) queue->headAddress) + topElementSize);

    // check if we reach the padding or the end
    unsigned initAddress = ((unsigned) queue) + sizeof(tpClosuresQueue_t);
    if (((unsigned)queue->headAddress) + queue->padding == initAddress + queue->size) {
        queue->headAddress = (void *) initAddress;
    }

    return SUCCESS_OP;
}

unsigned popTopElementAndDisownQueue (tpClosuresQueue_t * queue) {
    if (_checkIfValidAddress(queue) != SUCCESS_OP) {
       return INVALID_QUEUE_ADDRESS;
    }

    // Since there is only one owner, and only owners can peak,
    // this operation does not require to lock the queue

    unsigned currentCoreID;
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (currentCoreID));
    // Only the owner can peak. Check if owner
    if (queue->queueOwner != currentCoreID) {
       return NOT_OWNER;
    }

    // check if queue is empty
    if(isTpClosureQueueEmpty(queue)) {
        return EMPTY_QUEUE;
    }

    unsigned topElementSize = sizeof(genericTpClosure_t) + ((genericTpClosure_t *)queue->headAddress)->sizeOfArgs;

    queue->headAddress = (void *) (((unsigned) queue->headAddress) + topElementSize);

    // check if we reach the padding or the end
    unsigned initAddress = ((unsigned) queue) + sizeof(tpClosuresQueue_t);
    if (((unsigned)queue->headAddress) + queue->padding == initAddress + queue->size) {
        queue->headAddress = (void *) initAddress;
    }

    // Disown queue
    queue->queueOwner = 0;

    return SUCCESS_OP;
}
