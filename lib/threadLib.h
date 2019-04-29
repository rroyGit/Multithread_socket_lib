#ifndef THREADLIB_H_
#define THREADLIB_H_

#include <pthread.h>
#include "socketLib.h"

typedef struct {
    Connection* con;
    char* query;
    char* path;

} ThreadArg;

void* threadFunction (void* arg);

ThreadArg* getThreadArg (Connection* con, char *path, char* query);

pthread_t createThread (Connection* con, char* path, char* query);

void clearStdBuffer();


#endif