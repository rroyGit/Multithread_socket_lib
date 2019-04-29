#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit, atoi, malloc, free */
#include <string.h>

#include "fileLib.h"

void writeFile (char* sensorName, int sensorId, int value) {
    char fileName[30], buffer[10];
    sprintf(fileName, "%s_%i", sensorName, sensorId);
    sprintf(buffer, "%i\n", value);

    FILE* file = fopen(fileName, "a+");

    fwrite(buffer, sizeof(char), strlen(buffer), file);
    if (ferror (file)) printf ("Error Writing to %s_%i\n", sensorName, sensorId);

    fclose(file);
}