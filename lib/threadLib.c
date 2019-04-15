#include <time.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    Connection* con;
    char* query;
    char* path;

} ThreadArg;

ThreadArg* getThreadArg (Connection* con, char *path, char* query);
void clearStdBuffer();

void* threadFunction (void* arg) {
    Connection* con = ((ThreadArg*)arg)->con;
    char* path = ((ThreadArg*)arg)->path;
    char* query = ((ThreadArg*)arg)->query;
    
    char queryFinal[80], value[80];

    for (int i = 0; i < 50; i++) {
        startRequest(con);

        int id = query[10] - 48;
        
        srand(time(NULL));
        int valueInt = (rand() % 36) * id;
        
        //printf("%s - %d - %d\n", query, id, valueInt);
        
        strcpy(queryFinal, query);
        sprintf(value, "%d", valueInt);
        strcat(queryFinal, value);

        char* dummy[80] = {"GET", "localhost", "3001", path, queryFinal};

        setMessage(con, dummy);
        makeRequest(con);
        getResponse(&con);

        freeResources(con);
        closeSocket(con);
        unsigned int microseconds = 500000;
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