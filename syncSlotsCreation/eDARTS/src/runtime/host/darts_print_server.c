#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "darts_print_server.h"

struct {
    pthread_t printThread;
    int stillRunning;
} pthread_vars;

int start_printing_server()
{
    pthread_vars.stillRunning = 1;
    int rc = pthread_create(&(pthread_vars.printThread), NULL, printing_server, 0);
    if (rc) {
        printf("Error starting printer server pthread... \n");
        printf("-- Return code = %d",rc);
    }
    return rc;
}


void* printing_server(void *tid)
{
    printf("Starting the printer server ...\n");
    while (pthread_vars.stillRunning == 1)
    {
        // Do server stuff
        
    }
    pthread_exit(NULL);
}

void stop_printing_server()
{
    pthread_vars.stillRunning = 0;
}