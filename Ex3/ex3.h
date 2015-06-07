#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>   

// Const variable that is being used for arrays that I allocate
// memory for them, like reading the server's answer (where I dont
// really know the length of it).
#define MAX_BUFFER 5

#define MAX_HOST_NAME 100
#define TRUE 0
#define FALSE -1

// This function gets a line, extracts the suffix from it and returns
// it as a string.
char* getSuffix(char* line);

// This function gets the sockfd and the suffix of the line that we
// read (so we could know when to stop reading). It reads the 
// servers answer and returns it as a string.
char* readLineFromServer(int fd, char* ending);

// This function gets file path, sockfd and an addition that is needed
// according to what we want to write to the server (because not all the 
// needed data is stored within the file). Then it combines a line from
// the file along with the addition and sends to the server.
// Return values: -1=Error ; 0=Success ; 1=Finished reading from file.
int sendLineToServer(FILE* file, int fd, char* addition);

// This function checks the run command to see if we got all the 
// neccessary arguments (Which are: file path, server and port, though
// the port is not a must condition. if there's no port it sets it to 25
// as default). The return value is the port number.
int checkArgs(int argc, char **argv);

// Function that opens a socket connection between our client and
// the server. The return value is the sockfd. If failed somewhere it
// terminates the program.
int openSocketConnection(int port_number, char** argv, FILE* file);

// This function relays on the output of the function: sendLineToServer.
// It gets its output (returnVal), sockfd, file & suffix.
// This function checks if an error did occur, and if so, it free the 
// allocated memory of suffix, and close the file and socket connection.
void checkLineSentSuccessfully(int returnVal, int fd, FILE* file, char* suffix);

// This function gets the sockfd, finds the host name, and then writes
// to the socket 'helo [hostName]'.
void sendHelo(int fd);

// This function is charged on all the communication between our client
// and the server. It gets the sockfd and file and calls all the needed
// functions in order to talk with the server according to the file given.
void communicate(int fd, FILE* file);

// This function gets the response from the server (line), sockfd, file & suffix.
// It checks out the code in the beginning of the response in order to know if
// the server finished the last command it got succesful or if it returned an 
// error (any number above 400 is an error). In case of error it prints out the
// error, free allocated memory, close connections and terminate the program.
void checkServerResponse(char* line, int fd, FILE* file, char* suffix);