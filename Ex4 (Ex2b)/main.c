#include "ex2b.c"

int main(int argc, char **argv)
{
	FILE* file;
	Network* network;
	int success, attempts;
	
	if(argc != 4) {
		printf("Wrong number of arguments. Please enter the arguments in the following format:\n");
		printf("[Input File] [Source] [Number of connection attempts]\n");
		exit(EXIT_FAILURE);
	}

	// Open file for read.
	file=fopen(argv[1], "r");
	if (file == NULL) {
		perror("Error while opening the file\n");
		exit(EXIT_FAILURE);
	} 
	
	network = readFromFile(file, argv[2]);
	if (network==NULL) {
		freeNetwork(network);
		exit(EXIT_FAILURE);
	}

	fclose(file);
	
	success = initialize(network, argv[2]);
	if (success == FALSE) {
		printf("Failed initializing the needed data, Terminating Program.\n");
		freeNetwork(network);
		exit(EXIT_FAILURE);
	}
	
	attempts = atoi(argv[3]);
	success = createThreads(attempts, network);
	if (success == FALSE) {
		printf("Failed to create threads, Terminating Program.\n");
		freeNetwork(network);
		exit(EXIT_FAILURE);
	}

	freeNetwork(network);
	return 0;
}
