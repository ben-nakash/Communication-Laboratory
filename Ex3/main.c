#include "ex3.c"

int main(int argc, char **argv)
{
    int                 sockfd;
    unsigned short		port_number;
    FILE*				file;

    // Checking that the input is correct, and get the given port number from the command-line.
    port_number = checkArgs(argc, argv);

    // In case that the tester is changing the buffer size, I check that its > 0.
    if (MAX_BUFFER<1) {
    	printf("Error: Buffer cannot be less then 1.\n");
    	exit(EXIT_FAILURE);
    }

    // Opening the file in the given address for reading
    file = fopen(argv[1], "r");
    if (file == NULL) {
    	printf("Error: Cannot open file.\n");
    	exit(EXIT_FAILURE);	
    }

    // Opening a socket connection with the server
    sockfd = openSocketConnection(port_number, argv, file);
  	
  	// Communicating with the server according to the text file.
    communicate(sockfd, file);

    return(0);
}