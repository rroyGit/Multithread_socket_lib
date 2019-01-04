#include "socketLib.c"
#include "threadLib.c"

int main (int argc, char** args) {

    Connection* temp = newConnection();

    if (temp != NULL) {
        startRequest(temp);

        pthread_t p = pthread_self();
        printf("Main %lu\n", p);

        pthread_t temp1 = createThread(temp, "/temp", "?sensorId=1&value=");

        pthread_t temp2 = createThread(temp, "/temp", "?sensorId=1&value=");

        pthread_join(temp1, NULL);
        pthread_join(temp2, NULL);
        
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}