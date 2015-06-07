#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define TRUE 0
#define FALSE -1
#define ERROR -1
#define MAX_LENGTH 512

// This function gets a port number and open a socket connection for the server.
// Return the serverFD if succeeded, or -1(=ERROR) if failed.
int openServerSocketConnection(int port);

// This function simulates the server runtime. It goes on an endless loop and
// checks either a client tries to connect to it, to disconnect from it or send
// information through it to the other connected clients.
void startServerRun(int socketFD);

// This function creates a new connection with a new client.
// Return the new max number of connections.
int createNewConnection(int socketFD, fd_set* pReadFDs, int maxfd);

// This function checks if a connection is terminated or trying to send information
// to the other clients, and acts according to the needed operation.
void readLine(int fd, fd_set* pReadFDs, int maxfd, int serverFD);
