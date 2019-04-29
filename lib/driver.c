#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit, atoi, malloc, free */

#include "socketLib.h"
#include "threadLib.h"

#define MAX_SENSORS 4
#define MAX_SENSORS_PER 2
#define MAX_STRING_CHAR 80

void waitThreads ();
void doTemp (unsigned int* index);
void doSpeed (unsigned int* index);
void doDist (unsigned int* index);

char query[MAX_SENSORS_PER][MAX_STRING_CHAR] = {"?sensorId=1&value=", "?sensorId=2&value="};
pthread_t threadsRef[MAX_SENSORS];

int main (int argc, char** args) {
    unsigned int index = 0;

    pthread_t p = pthread_self();
    printf("Main thread %lu\n", p);
    
    doTemp(&index);
    doDist(&index);

    waitThreads();


    return EXIT_FAILURE;
}

void doTemp (unsigned int* index) {

    unsigned int tempIndex = *index;

    for (int i = 0; i < MAX_SENSORS_PER; tempIndex++, i++) {
        threadsRef[tempIndex] = createThread(newConnection(), "/temp", query[i]);
    }

    *index = tempIndex;
}

void doDist (unsigned int* index) {
    unsigned int tempIndex = *index;

    for (int i = 0; i < MAX_SENSORS_PER; tempIndex++, i++) {
        threadsRef[tempIndex] = createThread(newConnection(), "/dist", query[i]);
    }

    *index = tempIndex;
}

void doSpeed (unsigned int* index) {
    unsigned int tempIndex = *index;

    for (int i = 0; i < MAX_SENSORS_PER; tempIndex++, i++) {
        threadsRef[tempIndex] = createThread(newConnection(), "/speed", query[i]);
    }

    *index = tempIndex;
}

void waitThreads () {
    for (int i = 0; i < MAX_SENSORS; i++) {
        if (threadsRef[i] != 0) 
            pthread_join(threadsRef[i], NULL);
    }   
}