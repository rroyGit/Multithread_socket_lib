#ifndef SOCKETLIB_H_
#define SOCKETLIB_H_

#define RESPONSE_SIZE 4096
//#define CONTENT_TYPE "Content-Type: application/json; charset=utf-8"
#define CONTENT_TYPE "Content-Type: text/html; charset=utf-8"
#define NUM_ARGUMENTS 4
#define STRING_LENGTH 80
#define CONTENT_BODY "{\"name\": \"test18\", \"content\": \"FROM c program\"}"

struct hostent;
struct sockaddr_in;

// calculate size of message
int getMessageSize ();
// create message to be sent via HTTP
char* getMessage ();
// fill info in struct
void fillAddrStruct (struct sockaddr_in **server_addr, struct hostent* server, int port);
// connect the socket
void connectSocket (struct sockaddr_in** server_addr, int* socket_ref);
// create socket for connection
void createSocket (int* socket_ref);
// look up ip address
void setServer (struct hostent** server, char* host);
// send the request through write to socket
void sendRequest (char** message, int* socket_ref);
// receive the response
char* receiveResponse (int* socket_ref);
// get integer from char string
unsigned int getIntValue (char* string);
// clear STD buffer
void clearStdBuffer();

typedef struct {
    char *HTTP_METHOD;
    char *HOST, *PATH, *QUERY;
    unsigned int PORT;

    struct hostent *server;
    struct sockaddr_in *server_addr;

    char *message, *response;
    int socket_ref, bytes;
    unsigned int sent, received, total;
} Connection;

Connection* newConnection();

void initArgs (Connection** con, char** args);

unsigned int checkArguments(Connection* con, char** args);

void setMessage (Connection* con, char** args);

void startRequest (Connection* con);

char* getMessage (Connection* con);

void makeRequest (Connection* con);

void getResponse (Connection** con);

void freeResources (Connection* con);

void closeSocket (Connection* con);

#endif