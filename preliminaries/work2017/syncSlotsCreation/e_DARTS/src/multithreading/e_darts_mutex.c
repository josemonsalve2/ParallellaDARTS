#include "e_darts_mutex.h"

void darts_mutex_lock( e_mutex_t *mutex, unsigned baseAddr)
{
    darts_mutex_t *gmutex;
    gmutex = (darts_mutex_t *) ((unsigned)(mutex & 0x000FFFFF) + baseAddr);
    offset = 0x0;
    unsigned coreid;
    __asm__ __volatile__ ("movfs %0, coreid" : "=r" (coreid));

    do {
        val = coreid;
        __asm__ __volatile__(
                "testset    %[val], [%[gmutex], %[offset]]"
                : [val] "+r" (val)
                : [gmutex] "r" (gmutex), [offset] "r" (offset)
                : "memory");
    } while (val != 0);

    return;
}

void darts_mutex_unlock( e_mutex_t *mutex, unsigned baseAddr)
{
    const register uint32_t zero = 0;
    e_mutex_t *gmutex;
    gmutex = (darts_mutex_t *) ((unsigned)(mutex & 0x000FFFFF) + baseAddr);

    __asm__ __volatile__(
            "str    %[zero], [%[gmutex]]"
            : /* no outputs */
            : [zero] "r" (zero), [gmutex] "r" (gmutex)
            : "memory");

    return;
}
