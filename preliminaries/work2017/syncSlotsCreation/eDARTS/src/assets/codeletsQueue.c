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
#include "codeletsQueue.h"


void initCodeletsQueue( codeletsQueue_t * queue, unsigned int newSize, unsigned * newHeadAddress)
{
    queue->headAddress = newHeadAddress;
    queue->size = newSize;
    queue->currentTail=0;
    queue->curNumElements=0;
    queue->row = e_group_config.core_row;
    queue->col = e_group_config.core_col;
    queue->lockMutex = MUTEX_NULL;
}

unsigned pushCodeletQueue (codeletsQueue_t * queue, unsigned newCodelet)
{
    e_mutex_lock(queue->row, queue->col, &(queue->lockMutex));
    //check if full
    if (queue->curNumElements == queue->size)
    {
        e_mutex_unlock(queue->row, queue->col, &(queue->lockMutex));
        return 1;
    }
    //Insert in the current Tail
    queue->headAddress[queue->currentTail] = newCodelet;
    queue->currentTail = (queue->currentTail + 1) % queue->size;
    queue->curNumElements++;
    
    e_mutex_unlock(queue->row, queue->col, &(queue->lockMutex));
    return 0;
}

unsigned popCodeletQueue (codeletsQueue_t * queue, unsigned * popedCodelet)
{
    e_mutex_lock(queue->row, queue->col, &(queue->lockMutex));
    //check if empty
    if (queue->curNumElements == 0)
    {
        e_mutex_unlock(queue->row, queue->col, &(queue->lockMutex));
        popedCodelet = 0;
        return 1;
    }
    //Pop from head
    *popedCodelet = (queue->currentTail >= queue->curNumElements)? queue->headAddress[queue->currentTail - queue->curNumElements] : queue->headAddress[queue->currentTail - queue->curNumElements + queue->size];
    queue->curNumElements--;
    
    e_mutex_unlock(queue->row, queue->col, &(queue->lockMutex));
    return 0;
}

unsigned queueEmpty(codeletsQueue_t * queue)
{
    return queue->curNumElements;
}
