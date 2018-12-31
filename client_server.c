#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit, atoi, malloc, free */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <ctype.h> /* isdigit */

#define RESPONSE_SIZE 4096
<<<<<<< HEAD
#define PORT 3001
#define HOST "192.168.1.4"

#define HTTP_METHOD "GET"  //post: path(/docs/)  //get: path(/docs/test18)
#define PATH "/docs/test12/"
#define CONTENT_TYPE "Content-Type: application/json; charset=utf-8"
=======
//#define CONTENT_TYPE "Content-Type: application/json; charset=utf-8"
#define CONTENT_TYPE "Content-Type: text/html; charset=utf-8"
#define NUM_ARGUMENTS 4
#define STRING_LENGTH 80
>>>>>>> 8416d2b42d6b1c9aca5d727461b709ce00762cc9
#define CONTENT_BODY "{\"name\": \"test18\", \"content\": \"FROM c program\"}"


// calculate size of message
int getMessageSize ();
// create message to be sent via HTTP
char* getMessage ();
// fill info in struct
void fillAddrStruct (struct sockaddr_in** server_addr, struct hostent** server);
// connect the socket
void connectSocket (struct sockaddr_in** server_addr, int* socket_ref);
// create socket for connection
void createSocket (int* socket_ref);
// look up ip address
void setServer (struct hostent** server);
// send the request through write to socket
void sendRequest (char** message, int* socket_ref);
// receive the response
char* receiveResponse(int* socket_ref);


void initArgs (int* argc, char*** args);
char** getArguments(int* argc, char*** args);
int getIntValue (char** string);

char HTTP_METHOD[] = "GET";
char HOST[] = "localhost";
int PORT = 3001;
char PATH[] = "/temp";
char QUERY[] = "?sensorId=56&value=55";


int main (int argc, char ** args) {
    struct hostent *server = NULL;
    struct sockaddr_in* server_addr = NULL;

    int socket_ref, bytes, sent, received, total;
    char *message = NULL, *response = NULL;

    initArgs(&argc, &args);
    
    message = getMessage();

    createSocket(&socket_ref);

    setServer(&server);
   
    fillAddrStruct(&server_addr, &server);

    connectSocket(&server_addr, &socket_ref);

    sendRequest(&message, &socket_ref);

    response = receiveResponse(&socket_ref);
   
    // close the socket
    close(socket_ref);

    //process response
    printf("Response:\n%s\n", response);

    free(message);
    free(response);

    return EXIT_SUCCESS;
}

int getMessageSize () {
    int msg_size = 0;
    if (strcmp(HTTP_METHOD, "POST") == 0) {
        msg_size += strlen("%s %s%s HTTP/1.0\r\n");
        msg_size += strlen(HTTP_METHOD);// method
        msg_size += strlen(PATH);// path
        msg_size += strlen(CONTENT_TYPE) + strlen("\r\n"); // header

        msg_size += strlen("Content-Length: %d\r\n");
        msg_size += strlen("\r\n");
        msg_size += strlen(CONTENT_BODY); // body content
    } else if (strcmp(HTTP_METHOD, "GET") == 0) {
        msg_size += strlen("%s %s HTTP/1.0\r\n");
        msg_size += strlen(HTTP_METHOD);// method
        msg_size += strlen(PATH);// path
        msg_size += strlen(CONTENT_TYPE); // header
        msg_size += strlen(QUERY) + strlen("\r\n");
        
        msg_size += strlen("\r\n");
    }
    return msg_size;
}

char* getMessage () {
    // *Note - 2 new lines to end header
    char* message = malloc(getMessageSize());

    if (strcmp(HTTP_METHOD, "POST") == 0) {
        sprintf(message,"%s %s HTTP/1.0\r\n", HTTP_METHOD, PATH);
        strcat(message, CONTENT_TYPE); //header
        strcat(message, "\r\n");
        sprintf(message+strlen(message), "Content-Length: %lu\r\n", strlen(CONTENT_BODY));
        strcat(message, "\r\n");
        strcat(message, CONTENT_BODY);
    } else if (strcmp(HTTP_METHOD, "GET") == 0) {

        sprintf(message,"%s %s%s HTTP/1.0\r\n", HTTP_METHOD, PATH, QUERY);
        strcat(message, CONTENT_TYPE); // header
        strcat(message, "\r\n");

        strcat(message, "\r\n");
    }

    //sending this...
    //printf("---\n%s\n---\n", message);
    return message;
}

// fill info in struct
void fillAddrStruct (struct sockaddr_in **server_addr, struct hostent** server) {
    *server_addr = calloc(1, sizeof(*server_addr));
    
    (*server_addr)->sin_family = AF_INET;
    (*server_addr)->sin_port = htons(PORT);

    memcpy(&(*server_addr)->sin_addr.s_addr, (*server)->h_addr, (*server)->h_length);
}

void createSocket (int* socket_ref) {
    // create socket (domain, type, protocol)
    // http://man7.org/linux/man-pages/man2/socket.2.html
    if ((*socket_ref = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket not created yo");
        exit(EXIT_FAILURE);
    }
}

// connect the socket
void connectSocket (struct sockaddr_in** server_addr, int* socket_ref) {
    // https://linux.die.net/man/3/connect
    if (connect(*socket_ref, (struct sockaddr *) &(**server_addr), sizeof(**server_addr)) == -1) {
        perror("ERROR connecting yo");
        exit(EXIT_FAILURE);
    } else printf("Connected yo\n");
}

// look up ip address
void setServer (struct hostent** server) {
    if ((*server = gethostbyname(HOST)) == NULL) {
        perror("no such host yo");
        exit(EXIT_FAILURE);
    }
}

// send the request through write to socket
void sendRequest (char** message, int* socket_ref) {
    int total = strlen(*message);
    int bytes, sent = 0;
    do {
        bytes = write(*socket_ref, *message+sent, total-sent);
        if (bytes < 0) {
            perror("ERROR writing message to socket yo");
            exit(EXIT_FAILURE);
        }
        if (bytes == 0) break;
        sent += bytes;
    } while (sent < total);

    printf("Data sent...yo\n\n");
}

// receive the response
char* receiveResponse(int* socket_ref) {
    char* response = malloc(RESPONSE_SIZE);
    int total, bytes, received = 0;

    memset(response, 0, RESPONSE_SIZE);
    total = RESPONSE_SIZE - 1;
   
    do {
        bytes = read(*socket_ref, response + received, total - received);
        if (bytes < 0) {
            perror("ERROR reading response from socket yo");
            exit(EXIT_FAILURE);
        }
        
        if (bytes == 0) break;

        received += bytes;
    } while (received < total);

    if (received == total) {
        perror("ERROR storing complete response from socket yo");
        exit(EXIT_FAILURE);
    }

    return response;
}

void initArgs (int* argc, char*** args ) {
    char** arguments = NULL;
   
    if ((arguments = getArguments(argc, args)) != NULL) {
        // HOST
        if (arguments[0] != NULL) strcpy(HOST, arguments[0]);
        // PORT
        if (arguments[1] != NULL) PORT = getIntValue(&arguments[1]);
        // PATH
        if (arguments[2] != NULL) strcpy(PATH, arguments[2]);
        // QUERY
        if (arguments[3] != NULL) strcpy(QUERY, arguments[3]);

        printf("Using following values > HOST: %s PORT: %i PATH: %s QUERY: %s\n", HOST, PORT, PATH, QUERY);
    }
}

char** getArguments(int* argc, char*** args) {
    int numArguments = *argc;
    char** arguments = *args;
    char** returnList;
  
    if (numArguments == 1) {
        printf("Using default values > HOST: %s PORT: %i PATH: %s QUERY: %s\n", HOST, PORT, PATH, QUERY);
        return NULL;
    } else if (numArguments == NUM_ARGUMENTS + 1) {
        returnList = malloc(NUM_ARGUMENTS);

        for (int i = 0; i < NUM_ARGUMENTS; i++) {
            returnList[i] = malloc(STRING_LENGTH * sizeof(char));
            strcpy(returnList[i], arguments[i+1]);
        }
    } else {
        printf("Usage: <HOST> <PORT> <PATH> <QUERY>\n");
        exit(EXIT_FAILURE);
    }

    return returnList;
}

int getIntValue (char** string) {
  
    for (int i = 0; i < strlen(*string); i++) {
        if(!isdigit((*string)[i])) {
            printf("PORT conversion error - found non-integer digit\n");
            exit(EXIT_FAILURE);
        }
    }

    return atoi(*string);
}
