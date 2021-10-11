#include "codeletsQueue.h"

#define IS_ADDRESS_VALID(queue) ((((unsigned) queue) >> 20 == 0 )? false : true)
// This is because we need the pointer with the coreID in the address
#define GET_MUTEX_PTR(queue) ((darts_mutex_t *) (((unsigned)queue) + 2 * sizeof(void *) + sizeof(unsigned)));
// get queue available space needs to be changed; has an error where if the queue padding size is a multiple of the size of codelets
// then the tail address will end up the same as the head address on the last correct push and the macro reports then that the queue is empty
/*
#define GET_QUEUE_AVAILABLE_SPACE(queue) (  ((unsigned)queue->headAddress <= (unsigned)queue->tailAddress)?\
                                            (queue->size - (((unsigned)queue->tailAddress) - ((unsigned)queue->headAddress))):\
                                            ((((unsigned)queue->headAddress) - ((unsigned)queue->tailAddress))-1)  )
*/

#define GET_QUEUE_AVAILABLE_SPACE(queue) ( (queue->fullFlag)?\
						(0):\
						(((unsigned)queue->headAddress <= (unsigned)queue->tailAddress)?\
                                            (queue->size - (((unsigned)queue->tailAddress) - ((unsigned)queue->headAddress))):\
                                            ((((unsigned)queue->headAddress) - ((unsigned)queue->tailAddress))-1)) )


unsigned initCodeletsQueue( codeletsQueue_t * queue, unsigned int newSize)
{
    // Check the address of the queue to see if it is valid or not
    if (!IS_ADDRESS_VALID(queue))
        return CODELET_QUEUE_INVALID_QUEUE_ADDRESS;

    unsigned initAddress = (((unsigned) queue) + sizeof(codeletsQueue_t));

    queue->headAddress = (void *) initAddress;
    queue->tailAddress = (void *) initAddress;
    queue->fullFlag = 0;
    queue->size = newSize;
    queue->lockMutex = DARTS_MUTEX_NULL;

    return CODELET_QUEUE_SUCCESS_OP;
}

// pointer codelet to avoid calling memcpy
unsigned pushCodeletQueue (codeletsQueue_t * queue, codelet_t * newCodelet)
{
    // Check the address of the queue to see if it is valid or not
    if (!IS_ADDRESS_VALID(queue))
        return CODELET_QUEUE_INVALID_QUEUE_ADDRESS;

    darts_mutex_t *mutexPtr = GET_MUTEX_PTR(queue);

    darts_mutex_lock(mutexPtr);
    //check if full
    unsigned currSpace = GET_QUEUE_AVAILABLE_SPACE(queue);
    if (currSpace < sizeof(codelet_t)) {
        darts_mutex_unlock(mutexPtr);
        return CODELET_QUEUE_NOT_ENOUGH_SPACE;
    }
    else if (currSpace == sizeof(codelet_t)) { // room for exactly one more codelet
	queue->fullFlag = 1;
    }

    unsigned initAddress = (((unsigned) queue) + sizeof(codeletsQueue_t));
    unsigned endAddress = initAddress + queue->size;

    if (((unsigned)queue->tailAddress) + sizeof(codelet_t) < endAddress) {
        // Normal insertion Must be first to not pay a misprediction penalty in the pipeline
        codelet_t * tail = (codelet_t *) queue->tailAddress;
        *tail = *newCodelet;
        queue->tailAddress = (void *) (((unsigned)queue->tailAddress) + sizeof(codelet_t));
    } else {
        // Split insertion codelet will be inserted between the end and the beginning
        int i;
        char * tail = (char *) queue->tailAddress;
        char * codelet = (char *) newCodelet;
        for (i = 0; i < sizeof(codelet_t); i++) {
            *tail = *codelet;
            tail++;
            codelet++;
            if ((unsigned)tail == endAddress ) {
                tail = (char *) initAddress;
            }
        }
        queue->tailAddress = (unsigned *) tail;
    }

    darts_mutex_unlock(mutexPtr);
    return CODELET_QUEUE_SUCCESS_OP;
}

unsigned popCodeletQueue (codeletsQueue_t * queue, codelet_t * popedCodelet)
{
    // Check the address of the queue to see if it is valid or not
    if (!IS_ADDRESS_VALID(queue))
        return CODELET_QUEUE_INVALID_QUEUE_ADDRESS;

    darts_mutex_t *mutexPtr = GET_MUTEX_PTR(queue);

    darts_mutex_lock(mutexPtr);

    // Check if queue is empty
    if (GET_QUEUE_AVAILABLE_SPACE(queue) ==  queue->size) {
        darts_mutex_unlock(mutexPtr);
        popedCodelet = 0;
        return CODELET_QUEUE_EMPTY_QUEUE;
    }

    unsigned initAddress = (((unsigned) queue) + sizeof(codeletsQueue_t));
    unsigned endAddress = initAddress + queue->size;

    //Pop from head
    if (((unsigned)queue->headAddress) + sizeof(codelet_t) < endAddress) {
        // Normal pop Must be first to not pay a misprediction penalty in the pipeline
        codelet_t * head = (codelet_t *) queue->headAddress;
        *popedCodelet = *head;
        queue->headAddress = (void *) (((unsigned)queue->headAddress) + sizeof(codelet_t));
    } else {
        // Split insertion codelet will be inserted between the end and the beginning
        int i;
        char * head = (char *) queue->headAddress;
        char * codelet = (char *) popedCodelet;
        for (i = 0; i < sizeof(codelet_t); i++) {
            *codelet = *head;
            head++;
            codelet++;
            if ((unsigned)head == endAddress) {
                head = (char *)initAddress;
            }
        }
        queue->headAddress = (unsigned *) head;
    }

    darts_mutex_unlock(mutexPtr);
    return CODELET_QUEUE_SUCCESS_OP;
}

unsigned queueEmpty(codeletsQueue_t * queue)
{
    return (GET_QUEUE_AVAILABLE_SPACE(queue) ==  queue->size);
}
