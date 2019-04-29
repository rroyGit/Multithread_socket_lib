#include <stdio.h> /* printf, sprintf */
#include <stdint.h> /* int8_t, int32_t, uint8_t, etc */
#include <stdlib.h> /* exit, atoi, malloc, free */
#include <unistd.h> /* read, write, close, sleep */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <ctype.h> /* isdigit */
#include <pthread.h>

#include "socketLib.h"

Connection* newConnection () {
    Connection *connection = calloc (1, sizeof(*connection));
     
    if (connection == NULL) return NULL;

    (connection)->HTTP_METHOD = "GET";
    (connection)->HOST = "localhost";
    (connection)->PATH = "/temp";
    (connection)->QUERY = "?sensorId=56&value=55";
    (connection)->PORT = 3001;

    (connection)->server = NULL;
    (connection)->server_addr = NULL;
    (connection)->message = NULL;
    (connection)->response = NULL;

    (connection)->socket_ref = -1;
    return connection;
}

void setMessage (Connection* con, char** args) {
    initArgs(&con, args);
    con->message = getMessage(con);
}

void startRequest (Connection* con) {

    createSocket(&con->socket_ref);

    setServer(&(con->server), con->HOST);
   
    fillAddrStruct(&(con->server_addr), con->server, con->PORT);

    connectSocket(&(con->server_addr), &(con->socket_ref));
}

void makeRequest (Connection* con) {
    if (con->socket_ref != -1) {
        sendRequest(&(con->message), &(con->socket_ref));
    } else {
        printf("Socket isn't created - call startRequest() to do so.\n");
        pthread_exit(NULL);
    }
}

void getResponse (Connection** con) {
    (*con)->response = receiveResponse(&(*con)->socket_ref);

    //printf("Response:\n%s\n", (*con)->response);
}

void freeResources (Connection* con) {
    free(con->message);
    free(con->response);
}

void closeSocket (Connection* con) {
    close(con->socket_ref);
}

int getMessageSize (Connection* con) {
    int msg_size = 0;
    if (strcmp(con->HTTP_METHOD, "POST") == 0) {
        msg_size += strlen("%s %s%s HTTP/1.1\r\n");
        msg_size += strlen(con->HTTP_METHOD);// method
        msg_size += strlen(con->PATH);// path
        msg_size += strlen(CONTENT_TYPE) + strlen("\r\n"); // header

        msg_size += strlen("Content-Length: %d\r\n");
        msg_size += strlen("\r\n");
        msg_size += strlen(CONTENT_BODY); // body content
    } else if (strcmp(con->HTTP_METHOD, "GET") == 0) {
        msg_size += strlen("%s %s HTTP/1.1\r\n");
        msg_size += strlen(con->HTTP_METHOD); // method
        msg_size += strlen(con->PATH); // path
        msg_size += strlen(con->QUERY);
        msg_size += strlen(CONTENT_TYPE); // header
        msg_size += strlen("\r\nConnection: keep-alive");
        msg_size += strlen("\r\nContent-Length:	0");
        msg_size += strlen("\r\n");
        
        msg_size += strlen("\r\n");
    }
    return msg_size;
}

char* getMessage (Connection* con) {
    // *Note - 2 new lines to end header
    char* message = malloc(getMessageSize(con));

    if (strcmp(con->HTTP_METHOD, "POST") == 0) {
        sprintf(message,"%s %s HTTP/1.1\r\n", con->HTTP_METHOD, con->PATH);
        strcat(message, CONTENT_TYPE); //header
        strcat(message, "\r\n");
        sprintf(message+strlen(message), "Content-Length: %lu\r\n", strlen(CONTENT_BODY));
        strcat(message, "\r\n");
        strcat(message, CONTENT_BODY);
    } else if (strcmp(con->HTTP_METHOD, "GET") == 0) {

        sprintf(message,"%s %s%s HTTP/1.1\r\n", con->HTTP_METHOD, con->PATH, con->QUERY);
        strcat(message, CONTENT_TYPE); // header
        strcat(message, "\r\nConnection: keep-alive");
        strcat(message, "\r\nContent-Length: 0");

        strcat(message, "\r\n");
        strcat(message, "\r\n");
    }

    //sending this...
    clearStdBuffer();
    //printf("---\n%s\n---\n", message);
    return message;
}

// fill info in struct
void fillAddrStruct (struct sockaddr_in **server_addr, struct hostent* server, int port) {
    *server_addr = calloc(1, sizeof(*server_addr));
    
    (*server_addr)->sin_family = AF_INET;
    (*server_addr)->sin_port = htons(port);

    memcpy(&(*server_addr)->sin_addr.s_addr, server->h_addr, server->h_length);
}

void createSocket (int* socket_ref) {
    // create socket (domain, type, protocol)
    // http://man7.org/linux/man-pages/man2/socket.2.html

    if ((*socket_ref = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        clearStdBuffer();
        perror("Socket not created yo");
        pthread_exit(NULL);
    }
}

// connect the socket
void connectSocket (struct sockaddr_in** server_addr, int* socket_ref) {
    // https://linux.die.net/man/3/connect
    clearStdBuffer();

    if (connect(*socket_ref, (struct sockaddr *) &(**server_addr), sizeof(**server_addr)) == -1) {
        perror("ERROR connecting yo");
        pthread_exit(NULL);
    } //else printf("Connected yo\n");
}

// look up ip address
void setServer (struct hostent** server, char* host) {
    if ((*server = gethostbyname(host)) == NULL) {
        clearStdBuffer();
        perror("no such host yo");
        pthread_exit(NULL);
    }
}

// send sensor data request using write and reference to socket
void sendRequest (char** message, int* socket_ref) {
    unsigned int total = strlen(*message);
    unsigned int sent = 0;
    int bytes;
  
    clearStdBuffer();
    do {
        bytes = write(*socket_ref, *message + sent, total - sent);
        if (bytes < 0) {
            perror("ERROR writing message to socket yo");
            pthread_exit(NULL);
        }
        if (bytes == 0) break;
        sent += bytes;
    } while (sent < total);

   //printf("Data sent. %d\n\n", sent);
}

// receive response
char* receiveResponse(int* socket_ref) {
    char* response = malloc(RESPONSE_SIZE);
    unsigned int total, received = 0;
    int bytes;

    clearStdBuffer();
    memset(response, 0, RESPONSE_SIZE);
    total = RESPONSE_SIZE - 1;
   
    do {
        bytes = read(*socket_ref, response + received, total - received);
        if (bytes < 0) {
            perror("ERROR reading response from socket yo");
            pthread_exit(NULL);
        }

        received += bytes;
        if (received >= 239) break;

    } while (received < total);

    if (received == total) {
        perror("ERROR storing complete response from socket yo");
        pthread_exit(NULL);
    }

    //printf("%d\n", received);
    return response;
}

void initArgs (Connection** con, char** args) {

    if (checkArguments(*con, args)) {
        // HTTP METHOD 
        (*con)->HTTP_METHOD = args[0];
        // HOST
        (*con)->HOST = args[1];
        // PORT
        (*con)->PORT = getIntValue(args[2]);
        // PATH
        (*con)->PATH = args[3];
        // QUERY
        (*con)->QUERY = args[4];
    }
}

unsigned int checkArguments(Connection* con, char** args) {
    clearStdBuffer();
    if (args == NULL) {
        //printf("Using default values > HTTP-METHOD: %s HOST: %s PORT: %i PATH: %s QUERY: %s\n", 
            //con->HTTP_METHOD, con->HOST, con->PORT, con->PATH, con->QUERY);
        return (unsigned int) 0;
    } else {

        // TO-DO: error check for NULL in each index that is derefrenced

        //printf("Using these values >  HTTP-METHOD: %s HOST: %s PORT: %s PATH: %s QUERY: %s\n",
            //args[0], args[1], args[2], args[3], args[4]);
        return (unsigned int) 1;
    } 
}

unsigned int getIntValue (char* string) {
    clearStdBuffer();
    for (unsigned int i = 0; i < strlen(string); i++) {
        if(!isdigit((string)[i])) {
            printf("PORT conversion error - found non-integer digit\n");
            pthread_exit(NULL);
        }
    }
    
    if (atoi(string) < 0) {
        perror("Negative port number\n");
        pthread_exit(NULL);
    } else return (unsigned int) atoi(string);
}