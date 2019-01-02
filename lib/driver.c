#include "socketLib.c"

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** args) {

    Connection* a = newConnection();

    if (a != NULL) {
        int i;
        
        char query[80], value[80];
        startRequest(a);
        for (i = 0; i < 1; i++) {
            
            strcpy(query, "?sensorId=1&value=");
            sprintf(value, "%d", i);
            strcat(query, value);

            char* dummy[80] = {"GET", "localhost", "3001", "/temp", "?sensorId=1&value=71"};
            setMessage(a, dummy);
            makeRequest(a);
            
            getResponse(&a);
            sleep(2);
            char* dummy2[80] = {"GET", "localhost", "3001", "/temp", "?sensorId=1&value=72"};
            setMessage(a, dummy2);
            makeRequest(a);

            getResponse(&a);
            sleep(1);
            freeResources(a);
            closeSocket(a);
           
        }

    

        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}