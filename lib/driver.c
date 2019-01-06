#include "socketLib.c"
#include "threadLib.c"

#define MAX_SENSORS 12
#define MAX_SENSORS_PER 4
#define MAX_STRING_CHAR 80

void waitThreads ();
void doTemp (unsigned int* index);
void doSpeed (unsigned int* index);

char query[MAX_SENSORS_PER][MAX_STRING_CHAR] = {"?sensorId=1&value=", 
    "?sensorId=2&value=", "?sensorId=3&value=", "?sensorId=4&value="};
pthread_t threadsRef[MAX_SENSORS];

int main (int argc, char** args) {
    unsigned int index = 0;

    pthread_t p = pthread_self();
    printf("Main %lu\n", p);
    
    doTemp(&index);


    return EXIT_FAILURE;
}

void doTemp (unsigned int* index) {
    Connection* temp = newConnection();
    unsigned int tempIndex = *index;

    if (temp != NULL) {
        startRequest(temp);

       for ( ; tempIndex < MAX_SENSORS_PER; tempIndex++) {
            threadsRef[tempIndex] = createThread(temp, "/temp", query[tempIndex]);
        }
    }

    *index = tempIndex;
}

void doSpeed (unsigned int* index) {
    Connection* speed = newConnection();
    unsigned int tempIndex = *index;

    if (speed != NULL) {
        startRequest(speed);

       for ( ; tempIndex < 2*MAX_SENSORS_PER; tempIndex++) {
            threadsRef[tempIndex] = createThread(speed, "/speed", query[tempIndex]);
        }
    }

    *index = tempIndex;
}

void waitThreads () {
    for (int i = 0; i < MAX_SENSORS; i++) {
        if (threadsRef[i] != 0) 
            pthread_join(threadsRef[i], NULL);
    }   
}