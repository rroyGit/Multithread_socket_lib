#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit, atoi, malloc, free */
#include <string.h> /* memcpy, memset */
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "threadLib.h"
#include "fileLib.h"


void* threadFunction (void* arg) {
    Connection* con = ((ThreadArg*)arg)->con;
    char* path = ((ThreadArg*)arg)->path;
    char* query = ((ThreadArg*)arg)->query;
    
    char queryFinal[80], endQuery[80];

    char ids[] = "1234567890";
    char *fileName = path + 1; // skip the first '/' char of the route to get sensor name

    unsigned int microseconds = 600000; // 600 ms
    int indexId = strcspn(query, ids);
    int sensorId = (int) query[indexId] - 48;
    int seqNum = 0;

    for (int i = 0; i < 50; i++) {
        startRequest(con);

        int id = query[10] - 48;
        
        srand(time(NULL));
        int valueInt = (rand() % 36) * id;
        
        //printf("%s - %d - %d\n", query, id, valueInt);
        
        seqNum = (seqNum++) % 2;

        strcpy(queryFinal, query);
        sprintf(endQuery, "%i&seqNum=%i", valueInt, seqNum);
        strcat(queryFinal, endQuery);

        char* reqMessage[80] = {"GET", "localhost", "3001", path, queryFinal};

        setMessage(con, reqMessage);
        makeRequest(con);
        // getResponse(&con);

        writeFile(fileName, sensorId, valueInt);

        freeResources(con);
        closeSocket(con);
        
        usleep(microseconds);
    }

    pthread_t p = pthread_self();
    printf("Exiting thread %lu\n", p);
    
    return (void*) EXIT_SUCCESS;
}


pthread_t createThread (Connection* con, char* path, char* query) {
    ThreadArg* arg = getThreadArg(con, path, query);
    
    // flush out std buffer
    clearStdBuffer();

    pthread_t thread;
    if (!pthread_create(&thread, NULL, threadFunction, (void*) arg)) {
        printf("\tSub-thread %lu\n", thread);
    } else {
        perror("Unable to create thread!");
        exit(EXIT_FAILURE);
    }

    return thread;
}

ThreadArg* getThreadArg (Connection* con, char* path, char* query) {
    ThreadArg* arg = malloc(sizeof(*arg));
    arg->con = con;
    arg->query = query;
    arg->path = path;
    return arg;
}

void clearStdBuffer() {
    setvbuf(stdout, NULL, _IONBF, 0);
}