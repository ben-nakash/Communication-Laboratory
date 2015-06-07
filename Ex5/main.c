#include "ex5.c"

int main(int argc, char **argv)
{
	int serverFD, port;

	if(argc != 2)
	{
		printf("Wrong nuber of parameters. Please re-run only with port.\n");
		exit(EXIT_FAILURE);
	}

	port = atoi(argv[1]);
	serverFD = openServerSocketConnection(port);
	startServerRun(serverFD);

	return 0;
}