#include "tpClosuresQueue.h"

#if defined DEBUG_TP_CLOSURE_QUEUE && DEBUG_TP_CLOSURE_QUEUE == 1
#include "e_darts_print.h"
#endif

// Private function returns the available free space
unsigned calculateMaxFreeSpaceQueue(tpClosuresQueue_t * queue) {
    unsigned initAddress = ((unsigned) queue) + sizeof(tpClosuresQueue_t);
    if ((unsigned) queue->tailAddress >= (unsigned) queue->headAddress ) {
        // No padding included, No wrap of content
        unsigned leftOfHead = ((unsigned)queue->headAddress) - initAddress;
        unsigned rightOfTail = (initAddress + queue->size) - ((unsigned)queue->tailAddress);
        return (rightOfTail >= leftOfHead)? rightOfTail : leftOfHead; // return the max
    } else {
        return ((unsigned) queue->headAddress) - ((unsigned) queue->tailAddress);
    }
    return 0;
}

unsigned checkIfValidAddress (tpClosuresQueue_t * queue) {
    // We check if we are getting the whole address.
    if (((unsigned) queue) >> 20 == 0 ) {
        return TPC_QUEUE_INVALID_QUEUE_ADDRESS;
    }
    return TPC_QUEUE_SUCCESS_OP;
}

unsigned isTpClosureQueueEmpty(tpClosuresQueue_t * queue) {
    return (queue->headAddress == queue->tailAddress);
}

unsigned initTpClosuresQueue(tpClosuresQueue_t * queue, unsigned int newSize) {
    if (checkIfValidAddress(queue) != TPC_QUEUE_SUCCESS_OP) {
        return TPC_QUEUE_INVALID_QUEUE_ADDRESS;
    }

    void * init = (void *) (((unsigned) queue) + sizeof(tpClosuresQueue_t));

    //Initialize parameters
    queue->headAddress = init;
    queue->tailAddress = init;
    queue->padding = 0;
    queue->size = newSize;
    queue->lockMutex = DARTS_MUTEX_NULL;
#if defined DEBUG_TP_CLOSURE_QUEUE && DEBUG_TP_CLOSURE_QUEUE == 1
    e_darts_print("Queue Initialized queue= 0x%X, newHead = 0x%X, newTail = 0x%X, size = 0x%X \n",(unsigned) queue, (unsigned)queue->headAddress,(unsigned)queue->tailAddress,(unsigned)queue->size);
#endif

    return TPC_QUEUE_SUCCESS_OP;
}

unsigned pushTpClosureQueue (tpClosuresQueue_t * queue, genericTpClosure_t * newTpClosure) {
    if (checkIfValidAddress(queue) != TPC_QUEUE_SUCCESS_OP) {
        return TPC_QUEUE_INVALID_QUEUE_ADDRESS;
    }

    darts_mutex_t * mutexPtr = (darts_mutex_t *) (((unsigned)queue) + 2 * sizeof(void *) + 2*sizeof(unsigned));
    // We lock the queue
    darts_mutex_lock(mutexPtr);

    unsigned newTpSize = sizeof(genericTpClosure_t) + newTpClosure->sizeOfArgs;
    unsigned currentMaxFreeSpace = calculateMaxFreeSpaceQueue(queue);

    // Check if there is space. We must always be less than, otherwise, tail and head will meet
    if (newTpSize >= currentMaxFreeSpace) {
        darts_mutex_unlock(mutexPtr);
        return TPC_QUEUE_NOT_ENOUGH_SPACE;
    }

    // We check if head is greater than the tail. Normal insertion. There is padding already
    if ((unsigned) queue->tailAddress <= (unsigned) queue->headAddress) {
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
#if defined DEBUG_TP_CLOSURE_QUEUE && DEBUG_TP_CLOSURE_QUEUE == 1
    e_darts_print("Pushing inserted= %X, newHead = %x, newTail = %x, padding=%d \n",newTpSize, (unsigned)queue->headAddress,(unsigned)queue->tailAddress, queue->padding);
#endif
    // We unlock the queue
    // We lock the queue
    darts_mutex_unlock(mutexPtr);
    return TPC_QUEUE_SUCCESS_OP;
}

unsigned ownTpClosureQueue (tpClosuresQueue_t * queue) {
    if (checkIfValidAddress(queue) != TPC_QUEUE_SUCCESS_OP) {
        return TPC_QUEUE_INVALID_QUEUE_ADDRESS;
    }

    // get the mutex address
    darts_mutex_t * mutexPtr = (darts_mutex_t *) (((unsigned)queue) + 2 * sizeof(void *) + 2*sizeof(unsigned));
    // We lock the queue
    darts_mutex_lock(mutexPtr);
    // Check if it is owned already
    if (queue->queueOwner != 0) {
        // Queue Already Own
        darts_mutex_unlock(mutexPtr);
        return TPC_QUEUE_NOT_OWNER;
    }
    // Own queue
    unsigned currentCoreID;
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (currentCoreID));
    queue->queueOwner = currentCoreID;

#if defined DEBUG_TP_CLOSURE_QUEUE && DEBUG_TP_CLOSURE_QUEUE == 1
    e_darts_print("Queue 0x%X owned by core 0x%X \n",(unsigned)queue,queue->queueOwner);
#endif

    darts_mutex_unlock(mutexPtr);
    return TPC_QUEUE_SUCCESS_OP;
}

unsigned disownTpClosureQueue (tpClosuresQueue_t * queue) {
    if (checkIfValidAddress(queue) != TPC_QUEUE_SUCCESS_OP) {
        return TPC_QUEUE_INVALID_QUEUE_ADDRESS;
    }

    // Since there is only one owner, this operation does not require
    // to lock the queue


    // Only the owner can disown. Check if it is the owner
    unsigned currentCoreID;
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (currentCoreID));
    if (queue->queueOwner != currentCoreID) {
        return TPC_QUEUE_NOT_OWNER;
    }

#if defined DEBUG_TP_CLOSURE_QUEUE && DEBUG_TP_CLOSURE_QUEUE == 1
    e_darts_print("Queue 0x%X disowned by core 0x%X \n",(unsigned)queue,queue->queueOwner);
#endif

    // Disown queue
    queue->queueOwner = 0;

    return TPC_QUEUE_SUCCESS_OP;
}

unsigned peakTopElement(tpClosuresQueue_t * queue, genericTpClosure_t ** topElementPointer) {
    if (checkIfValidAddress(queue) != TPC_QUEUE_SUCCESS_OP) {
           return TPC_QUEUE_INVALID_QUEUE_ADDRESS;
       }

    // Since there is only one owner, and only owners can peak,
    // this operation does not require to lock the queue

    unsigned currentCoreID;
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (currentCoreID));
    // Only the owner can peak. Check if owner
    if (queue->queueOwner != currentCoreID) {
       *topElementPointer = 0;
       return TPC_QUEUE_NOT_OWNER;
    }

    // check if queue is empty
    if(isTpClosureQueueEmpty(queue)) {
        *topElementPointer = 0;
        return TPC_QUEUE_EMPTY_QUEUE;
    }

    // return topElementPointer
    *topElementPointer = (genericTpClosure_t *) queue->headAddress;

    return TPC_QUEUE_SUCCESS_OP;
}

unsigned popTopElementQueue (tpClosuresQueue_t * queue) {
    if (checkIfValidAddress(queue) != TPC_QUEUE_SUCCESS_OP) {
           return TPC_QUEUE_INVALID_QUEUE_ADDRESS;
    }

    // Since there is only one owner, and only owners can peak,
    // this operation does not require to lock the queue

    unsigned currentCoreID;
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (currentCoreID));
    // Only the owner can peak. Check if owner
    if (queue->queueOwner != currentCoreID) {
       return TPC_QUEUE_NOT_OWNER;
    }

    // if the queue is empty, let's put the head at the beginnig of the memory space
    // but since we are gonna modify the tail we need to lock
    // get the mutex address
    darts_mutex_t * mutexPtr = (darts_mutex_t *) (((unsigned)queue) + 2 * sizeof(void *) + 2*sizeof(unsigned));
    // We lock the queue
    darts_mutex_lock(mutexPtr);

    // check if queue is empty
    if(isTpClosureQueueEmpty(queue)) {
        return TPC_QUEUE_EMPTY_QUEUE;
    }

    unsigned topElementSize = sizeof(genericTpClosure_t) + ((genericTpClosure_t *)queue->headAddress)->sizeOfArgs;

    queue->headAddress = (void *) (((unsigned) queue->headAddress) + topElementSize);

    // check if we reach the padding or the end
    unsigned initAddress = ((unsigned) queue) + sizeof(tpClosuresQueue_t);
    if (((unsigned)queue->headAddress) + queue->padding == initAddress + queue->size) {
        queue->headAddress = (void *) initAddress;
        queue->padding = 0;
    }


    if (queue->headAddress == queue->tailAddress) {
        queue->headAddress = queue->tailAddress = (void *) initAddress;
        queue->padding = 0;
    }
#if defined DEBUG_TP_CLOSURE_QUEUE && DEBUG_TP_CLOSURE_QUEUE == 1
    e_darts_print("Popping removed= %X, newHead = %x, newTail = %x, padding=%d \n",topElementSize, (unsigned)queue->headAddress,(unsigned)queue->tailAddress, queue->padding);
#endif

    darts_mutex_unlock(mutexPtr);
    return TPC_QUEUE_SUCCESS_OP;
}

unsigned popTopElementAndDisownQueue (tpClosuresQueue_t * queue) {
    if (checkIfValidAddress(queue) != TPC_QUEUE_SUCCESS_OP) {
       return TPC_QUEUE_INVALID_QUEUE_ADDRESS;
    }

    // Since there is only one owner, and only owners can peak,
    // this operation does not require to lock the queue

    unsigned currentCoreID;
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (currentCoreID));
    // Only the owner can peak. Check if owner
    if (queue->queueOwner != currentCoreID) {
       return TPC_QUEUE_NOT_OWNER;
    }

    // get the mutex address
    darts_mutex_t * mutexPtr = (darts_mutex_t *) (((unsigned)queue) + 2 * sizeof(void *) + 2*sizeof(unsigned));
    // We lock the queue
    darts_mutex_lock(mutexPtr);


    // check if queue is empty
    if(isTpClosureQueueEmpty(queue)) {
        return TPC_QUEUE_EMPTY_QUEUE;
    }

    unsigned topElementSize = sizeof(genericTpClosure_t) + ((genericTpClosure_t *)queue->headAddress)->sizeOfArgs;

    queue->headAddress = (void *) (((unsigned) queue->headAddress) + topElementSize);

    // check if we reach the padding or the end
    unsigned initAddress = ((unsigned) queue) + sizeof(tpClosuresQueue_t);
    if (((unsigned)queue->headAddress) + queue->padding == initAddress + queue->size) {
        queue->headAddress = (void *) initAddress;
        queue->padding = 0;
    }


    if (queue->headAddress == queue->tailAddress) {
        queue->headAddress = queue->tailAddress = (void *) initAddress;
        queue->padding = 0;
    }

#if defined DEBUG_TP_CLOSURE_QUEUE && DEBUG_TP_CLOSURE_QUEUE == 1
    e_darts_print("Popping removed= %X, newHead = %x, newTail = %x, padding=%d \n",topElementSize, (unsigned)queue->headAddress,(unsigned)queue->tailAddress, queue->padding);
    e_darts_print("Queue 0x%X disowned by core 0x%X \n",(unsigned)queue,queue->queueOwner);
#endif
    darts_mutex_unlock(mutexPtr);

    // Disown queue
    queue->queueOwner = 0;

    return TPC_QUEUE_SUCCESS_OP;
}
