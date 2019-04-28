#include <stdio.h>

void writeFile (char* sensorName, int sensorId, int value) {
    char fileName[30], buffer[10];
    sprintf(fileName, "%s_%i", sensorName, sensorId);
    sprintf(buffer, "%d\n", value);

    FILE* file = fopen(fileName, "a");

    fwrite(buffer, sizeof(char), sizeof(buffer), file);
    if (ferror (file)) printf ("Error Writing to %s_%i\n", sensorName, sensorId);

    fclose(file);
}