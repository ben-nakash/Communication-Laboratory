#include "ex5.h"


int openServerSocketConnection(int port)
{
	int serverFD, reuse=1;
	struct sockaddr_in server_Addr;

	memset(&server_Addr, 0, sizeof(server_Addr));
	server_Addr.sin_family = AF_INET;
	server_Addr.sin_port = htons(port);
	server_Addr.sin_addr.s_addr = INADDR_ANY;
	
	serverFD = socket(AF_INET , SOCK_STREAM, 0);
	if (serverFD == -1) 
		return ERROR;

	if (bind(serverFD, (struct sockaddr *)&server_Addr, sizeof(server_Addr)) == -1)
	{
		printf("Error bind failed\n");
		return ERROR;
	}

	if (listen(serverFD,10) == -1)
	{
		printf("Error listen failed\n");
		return ERROR;
	}

	if (setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &reuse,sizeof(reuse))<0)
	{
		printf("Error\n");
		return ERROR;
	}

	return serverFD;
}


void startServerRun(int serverFD)
{
	int i, maxfd;
	fd_set readFDs;
	fd_set tempSet;
	fd_set *pReadFDs = &readFDs;

	FD_ZERO(&readFDs); 
	FD_ZERO(&tempSet);
	
	maxfd = serverFD;

	FD_SET(serverFD, &readFDs);
	
	while(1) 
	{
		tempSet = readFDs;

		if (select(maxfd+1, &tempSet, 0, 0, 0) < 0) {
			printf("Error in select.\n");
			exit(EXIT_FAILURE);
		}

		for(i=0 ; i<maxfd+1 ; i++)
		{
			if(FD_ISSET(i, &tempSet)) 
			{
				// In this case it means there's someone that trying to connect to our server.
				if(i == serverFD) 
					maxfd = createNewConnection(serverFD, pReadFDs, maxfd);
				// In this case it means that either someone tries to dissconnect from our server
				// or someone is sending information that we need to pass to the other clients.
				else 
					readLine(i, pReadFDs, maxfd, serverFD);
			}
		}
	}
}


void readLine(int fd, fd_set* pReadFDs, int maxfd, int serverFD)
{
	int nbytes=0, i;
	char buffer[MAX_LENGTH];
	memset(&buffer[0], 0, MAX_LENGTH);

	nbytes = read(fd, buffer,sizeof(buffer));

	// In this case it means that there was an error or someone tries to dissconnect.
	if (nbytes <= 0) 
	{
		if (nbytes==0)
			printf("Socket number %d disconnected.\n", fd);
		else
			printf("Error while trying to read.\n");
		close(fd);
		FD_CLR(fd, pReadFDs);
	}
	// In this case it means that someone is sending data that we need to transfer to
	// all the other clients.
	else 
	{
		for (i=0 ; i<maxfd+1 ; i++)
		{
			if(FD_ISSET(i, pReadFDs) && i!=serverFD && i!=fd) 
			{
				if(write(i, buffer, strlen(buffer)) < 0)
				{
					perror("Error while trying to send.\n");
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	return;
}


int createNewConnection(int serverFD, fd_set* pReadFDs, int maxfd)
{
	int newSocket;

	if ((newSocket = accept(serverFD, (struct sockaddr *)NULL, NULL))<0) 
	{
		close(serverFD);
		printf("Error while trying to do 'accept'\n");
		exit(EXIT_FAILURE);
	}				
	else 
	{
		FD_SET(newSocket, pReadFDs);
		if(newSocket > maxfd)
			maxfd=newSocket;
	}
	return maxfd;
}
