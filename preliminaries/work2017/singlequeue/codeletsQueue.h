#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"
#include "common.h"

typedef struct codeletsQueue_s {
    unsigned int size;
    unsigned int curNumElements;
    unsigned * headAddress;
    unsigned int currentTail;
    e_mutex_t lockMutex;
} codeletsQueue_t;


void initCodeletsQueue( codeletsQueue_t * queue, unsigned int newSize, unsigned * newHeadAddress)
{
    queue->headAddress = newHeadAddress;
    queue->size = newSize;
    queue->currentTail=0;
    queue->curNumElements=0;
    queue->lockMutex = MUTEX_NULL;
}

//Return 1 if full, return 0 if successful push. It is responsability of the caller to deal with it
unsigned pushCodeletQueue (codeletsQueue_t * queue, unsigned pusherRow, unsigned pusherCol, unsigned newCodelet)
{
    while ( !e_mutex_trylock(pusherRow, pusherCol, &(queue->lockMutex)) );
    //check if full
    if (queue->curNumElements == queue->size)
    {
        e_mutex_unlock(pusherRow, pusherCol, &(queue->lockMutex));
        return 1;
    }
    //Insert in the current Tail
    queue->headAddress[queue->currentTail] = newCodelet;
    queue->currentTail = (queue->currentTail + 1) % queue->size;
    queue->curNumElements++;
    
    e_mutex_unlock(pusherRow, pusherCol, &(queue->lockMutex));
    return 0;
}


//Return 1 if empty, return 0 if something popped
unsigned popCodeletQueue (codeletsQueue_t * queue, unsigned popRow, unsigned popCol, unsigned * popedCodelet)
{
    while ( !e_mutex_trylock(popRow, popCol, &(queue->lockMutex)) );
    //check if empty
    if (queue->curNumElements == 0)
    {
        e_mutex_unlock(popRow, popCol, &(queue->lockMutex));
        return 1;
    }
    //Pop from head
    *popedCodelet = (queue->currentTail >= queue->curNumElements)? queue->headAddress[queue->currentTail - queue->curNumElements] : queue->headAddress[queue->currentTail - queue->curNumElements + queue->size];
    queue->curNumElements--;
    
    e_mutex_unlock(popRow, popCol, &(queue->lockMutex));
    return 0;
}

