#include "ex2b.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t calcCondVar = PTHREAD_COND_INITIALIZER;
pthread_cond_t sendCondVar = PTHREAD_COND_INITIALIZER;
int finishFlag = FALSE;
int* whoSentDV;
int* wait;

Network* readFromFile(FILE* file, char* sourceName)
{
	int i, numOfRouters, numOfEdges=0, routerPort, edgeWeight;
	char *token;
	char line[MAX_LENGTH], routerName[MAX_LENGTH], routerIP[MAX_LENGTH];
	Network *network;
	Router *newRouter, *temp1, *temp2;
	Edge *newEdge1, *newEdge2;

	if (file==NULL)
		return NULL;

	// Reading the first line of the file with the number of routers
	fgets(line, MAX_LENGTH, file);
	token = strtok(line, "");
	numOfRouters = atoi(token);

	// Checking how many edges we have.
	for (i=0 ; i<numOfRouters ; i++) {
		fgets(line, MAX_LENGTH, file);
	}
	while (fgets(line, MAX_LENGTH, file) != NULL) 
		numOfEdges++;
	numOfEdges *= 2; // Multiply by 2 because the edges are from both sides.

	rewind(file);	// Re-positioning to the begining of the file. 

	network = createNetwork(numOfEdges, numOfRouters);
	if (network == NULL) {
		printf("Error while attemting to create a network.\n");
		return NULL;
	}

	fgets(line, MAX_LENGTH, file);

	// Creating the routers structs
	for (i=0 ; i<numOfRouters ; i++)
	{
		fgets(line, MAX_LENGTH, file);
		token = strtok(line, " ");
		strcpy(routerName, token);
		token = strtok(NULL, " ");
		strcpy(routerIP, token);
		token = strtok(NULL, " ");

		routerPort = atoi(token);
		newRouter = createRouter(routerName, routerIP, routerPort);
		newRouter = addRouterToNetwork(newRouter, network);
	}

	// Set the source of the graph.
	network->source = findRouterByName(sourceName, network);

	// Creating the edges
	while (fgets(line, MAX_LENGTH, file) != NULL)
	{
		token = strtok(line, " ");
		strcpy(routerName, token);
		temp1 = findRouterByName(routerName, network);
		token = strtok(NULL, " ");
		strcpy(routerName, token);
		temp2 = findRouterByName(routerName, network);
		token = strtok(NULL, " ");
		edgeWeight = atoi(token);

		newEdge1 = createEdge(temp1, temp2, edgeWeight);
		newEdge2 = createEdge(temp2, temp1, edgeWeight);
		addEdgeToNetwork(newEdge1, network);
		addEdgeToNetwork(newEdge2, network);
	}

	// Check the number of neighboors of each one of the routers.
	for(i=0 ; i<numOfRouters ; i++)
		checkNumOfNeighbors(network, network->routers[i]);

	return network;
}


Router* createRouter(char* rName, char* rIP, int rPort)
{
	if (rName == NULL || rIP == NULL || rPort <= 0)
		return NULL;

	Router* newRouter = (Router*)malloc(sizeof(Router));
	if (newRouter == NULL)
		return NULL;

	newRouter->name = (char*)malloc(sizeof(char)*(strlen(rName)+1));
	if (newRouter->name == NULL) {
		freeRouter(newRouter);
		return NULL;
	}
	strcpy(newRouter->name, rName);

	newRouter->ip = (char*)malloc(sizeof(char)*(strlen(rIP)+1));
	if (newRouter->ip == NULL) {
		freeRouter(newRouter);
		return NULL;
	}
	strcpy(newRouter->ip, rIP);

	newRouter->dv = NULL;
	newRouter->via = NULL;
	newRouter->neighbors = NULL;
	newRouter->port = rPort;
	newRouter->numOfNeighbors=0;

	return newRouter;
}


Network* createNetwork(int numOfEdges, int numOfRouters)
{
	int i;
	Network* network;

	network = (Network*)malloc(sizeof(Network));
	if (network == NULL)
		return NULL;

	// Allocating memory to the routers list & the edges list.
	network->routers = (Router**)malloc(numOfRouters*sizeof(Router*));
	if (network->routers == NULL)
		return NULL;
	
	network->edges = (Edge**)malloc(numOfEdges*sizeof(Edge*));
	if (network->edges == NULL)
		return NULL;
	
	// Going through those lists in order to avoid any junk that might be there.
	for(i=0 ; i<numOfRouters ; i++)
		network->routers[i]=NULL;
	for(i=0 ; i<numOfEdges ; i++) 
		network->edges[i]=NULL;
	
	network->numOfEdges=numOfEdges;
	network->numOfRouters=numOfRouters;
	return network;
}


Router* addRouterToNetwork(Router* router, Network* network)
{
	int i=0;
	if (router==NULL || network==NULL) {
		printf("Error - NULL pointer in 'addRouterToGraph'\n");
		return NULL;
	}

	for (i=0 ; i<network->numOfRouters ; i++)
	{
		// If nothing exists on this list index, add it.
		if (network->routers[i] == NULL) {
			network->routers[i] = router;
			return router;
		}
		// If the given router is already exist, it free the given one, and return the already existing one.
		if (network->routers[i] != NULL && strcmp(router->name, network->routers[i]->name)==0) {
			freeRouter(router);		
			router=NULL;
			return network->routers[i];	
		}
	}
	return NULL;	// If it got here there have been some kind of fault or error so it returns NULL.
}


void freeRouter(Router* router)
{
	if (router==NULL)
		return;
	if (router->name != NULL) {
		free(router->name);
		router->name=NULL;
	}
	if (router->ip != NULL) {
		free(router->ip);
		router->ip = NULL;
	}
	if (router->dv != NULL) {
		free(router->dv);
		router->dv=NULL;
	}
	if (router->via != NULL) {
		free(router->via);
		router->via=NULL;
	}
	if (router->neighbors != NULL) {
		free(router->neighbors);
		router->neighbors=NULL;
	}
	free(router);
	router=NULL;
}


Router* findRouterByName(char* name, Network* network)
{
	int i=0;
	if (name==NULL || network==NULL)
		return NULL;
	while(network->routers[i] != NULL)
	{
		if (strcmp(network->routers[i]->name, name)==0)
			return network->routers[i];
		i++;
	}
	return NULL;
}


Edge* createEdge(Router* src, Router* dst, int weight)
{
	Edge* newEdge;

	if (src==NULL || dst==NULL || weight<=0)
		return NULL;
	
	newEdge = (Edge*)malloc(sizeof(Edge));
	if (newEdge == NULL)
		return NULL;
	newEdge->source = src;
	newEdge->destination = dst;
	newEdge->weight = weight;
	return newEdge;
}


void addEdgeToNetwork(Edge* edge, Network* network)
{
	int i;
	if (edge==NULL || edge->source==NULL || edge->destination==NULL || network==NULL) {
		printf("NULL pointer in 'addEdgeToNetwork'\n");
		return;
	}
	
	for (i=0 ; i<network->numOfEdges ; i++)
	{
		// Checking if such edge is already exist in the network. If do, free this edge and finish.
		if(network->edges[i]!=NULL && strcmp(edge->source->name, network->edges[i]->source->name)==0 &&
			strcmp(edge->destination->name, network->edges[i]->destination->name)==0)
		{
			freeEdge(edge);
			return;
		}
		// If it doesn't exist and there's nothing on that index location, add the edge to the network.
		else if(network->edges[i]==NULL) {
			network->edges[i] = edge;
			return;	
		}
	}	
	
}		


void freeEdge(Edge* edge)
{
	if (edge==NULL)
		return;
	edge->source=NULL;
	edge->destination=NULL;
	free(edge);
	edge=NULL;
}


void freeNetwork(Network* network)
{
	int i;
	if (network==NULL)
		return;
	for (i=0 ; i<network->numOfRouters ; i++) {
		freeRouter(network->routers[i]);
		network->routers[i]=NULL;
	}
	for (i=0 ; i<network->numOfEdges ; i++) {
		freeEdge(network->edges[i]);
		network->edges[i]=NULL;
	}
	if (network->routers != NULL) {
		free(network->routers);
		network->routers=NULL;
	}
	if (network->edges != NULL) {
		free(network->edges);
		network->edges=NULL;
	}
	if (wait != NULL)
		free(wait);
	if (whoSentDV != NULL)
		free(whoSentDV);

	free(network);
	network=NULL;
}


int initializeDV(Network* network)
{
	int numOfRouters, i, edgeWeight;

	if (network==NULL)
		return FALSE;

	numOfRouters = network->numOfRouters;
	network->source->dv = (int*)malloc(numOfRouters*sizeof(int));
	if (network->source->dv == NULL)
		return FALSE;

	for (i=0; i<numOfRouters ; i++)
	{
		if (strcmp(network->source->name, network->routers[i]->name)==0) {
			network->source->dv[i] = 0;
			continue;
		}
		else
		{
			edgeWeight = areNeighboors(network, network->routers[i]);
			if (edgeWeight==FALSE)
				network->source->dv[i] = INFINITI;
			else
				network->source->dv[i] = edgeWeight;
		}
	}
	return 0;
}


int initializeVIA(Network* network)
{
	int i, numOfRouters;

	if (network==NULL)
		return FALSE;

	numOfRouters = network->numOfRouters;

	network->source->via = (char**)malloc(numOfRouters*sizeof(char*));
	if (network->source->via == NULL) 
		return FALSE;

	for(i=0 ; i<numOfRouters ; i++)
		network->source->via[i] = network->routers[i]->name;

	return 0;
}


int initializeNeighbors(Network* network)
{
	int i, j=0, numOfRouters, numOfNeighbors=0, areNeighbors;

	if (network==NULL)
		return FALSE;

	numOfRouters = network->numOfRouters;

	for(i=0 ; i<numOfRouters ; i++) 
		if (network->source->dv[i]!=0 && network->source->dv[i]!=INFINITI)
			numOfNeighbors++;
	network->source->numOfNeighbors = numOfNeighbors;

	network->source->neighbors = (char**)malloc((numOfNeighbors)*sizeof(char*));
	if (network->source->neighbors == NULL)
		return FALSE;

	for(i=0 ; i<numOfNeighbors ; i++)
	{
		areNeighbors = areNeighboors(network, network->routers[j]);
		if (areNeighbors == FALSE) {
			i--;
			j++;
			continue;
		}
		else {
			network->source->neighbors[i] = network->routers[j]->name;
			j++;
		}
	}

	return 0;
}

int initialize(Network* network, char* sourceName)
{
	int succeeded;
	Router* source;

	if (network==NULL || sourceName==NULL)
		return FALSE;

	source = findRouterByName(sourceName, network);
	if (source==NULL)
		return FALSE;

	succeeded = initializeDV(network);
	if (succeeded == FALSE)
		return FALSE;

	succeeded = initializeVIA(network);
	if (succeeded == FALSE)
		return FALSE;

	succeeded = initializeNeighbors(network);
	if (succeeded == FALSE)
		return FALSE;

	return 0;
}


int areNeighboors(Network* network, Router* router)
{
	int i;
	if(network==NULL || router==NULL)
		return FALSE;

	for (i=0 ; i<network->numOfEdges ; i++) {
		if (strcmp(network->edges[i]->source->name, network->source->name)==0 && 
			strcmp(network->edges[i]->destination->name, router->name)==0)
			return network->edges[i]->weight;
	}
	return FALSE;
}


void checkNumOfNeighbors(Network* network, Router* router)
{
	int i, numOfEdges = network->numOfEdges;

	if (network==NULL || router==NULL)
		return;

	for(i=0 ; i<numOfEdges ; i++)
	{
		if(strcmp(network->edges[i]->source->name, router->name)==0)
			router->numOfNeighbors++;
	}
}


int senderSocketConnect(ThreadData* data)
{
	int 	sockfd, connects, i, port=0, attempts;
	struct 	sockaddr_in serv_addr;
	struct 	hostent *server;
	Router* connectToNeighbor;

	if (data==NULL)
        return FALSE;
	
	connectToNeighbor = findRouterByName(data->connectToNeighbor, data->network);
	if (connectToNeighbor==NULL)
		return FALSE;

	port += connectToNeighbor->port;
	for(i=0 ; i<strlen(data->network->source->name) ; i++)
		port += data->network->source->name[i];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		return FALSE;

	server = gethostbyname(connectToNeighbor->ip);
	if (server == NULL)
		return FALSE;
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	attempts = data->attempts;
	while(1) 
	{
		connects = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		if (connects < 0) 
		{
			if (attempts>0) {
				sleep(2);
				attempts--;
				continue;
			}
			printf("Error: Can't connect to server\n");
			return FALSE;
		}
		break;
	}
	return sockfd;
}


int receiverSocketConnect(ThreadData* data)
{
	struct sockaddr_in server_Addr;
	int socketFD, connFD=0, port=0, attempts, i, reuse=1;

	if (data==NULL)
		return FALSE;
	
    // Calculating the needed port.
	port += data->network->source->port;
	for(i=0 ; i<strlen(data->connectToNeighbor) ; i++)
		port += data->connectToNeighbor[i];

	memset(&server_Addr, 0, sizeof(server_Addr));
	server_Addr.sin_family = AF_INET;
	server_Addr.sin_port = htons(port);
	server_Addr.sin_addr.s_addr = htons(INADDR_ANY);
	
	socketFD = socket(AF_INET , SOCK_STREAM, 0);
	if (socketFD == -1) 
		return FALSE;

	if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &reuse,sizeof(reuse))<0)
	{
		printf("Error\n");
		return FALSE;
	}
	if (bind(socketFD, (struct sockaddr *)&server_Addr, sizeof(server_Addr)) == -1)
	{
		printf("Error bind failed\n");
		return FALSE;
	}

	if (listen(socketFD,10) == -1)
	{
		printf("Error listen failed\n");
		return FALSE;
	}

	attempts = data->attempts;
	while (1)
	{
		connFD = accept(socketFD, (struct sockaddr *)NULL, NULL);
		if (connFD < 0) 
		{
			if (attempts > 0) {
	    		sleep(2);
	    		attempts--;
	    		continue;
	    	}
			printf("Error - accept failed\n");
			close(socketFD);
			return FALSE;
		}
		break;
	}
	return connFD;
}


ThreadData* initializeThreadData(int attempts, Network* network)
{
	int i, j=0, numOfNeighbors;
	ThreadData* data;

	if (network==NULL || attempts<1)
		return NULL;

	numOfNeighbors = network->source->numOfNeighbors;
	data = (ThreadData*)malloc(sizeof(ThreadData)*(numOfNeighbors+1));
	if (data==NULL)
		return NULL;

	whoSentDV = (int*)malloc(network->numOfRouters*sizeof(int));
	if (whoSentDV==NULL) {
		//free(data);
		return NULL;
	}
	initializeWhoSentDV(network);

	wait = (int*)malloc(network->source->numOfNeighbors*sizeof(int));
	if (wait==NULL) {
		free(data);
		return NULL;
	}
	for(i=0 ; i<network->source->numOfNeighbors ; i++)
		wait[i]=1;

	for (i=0 ; i<numOfNeighbors ; i++)
	{	
			if(strcmp(network->source->via[j], network->source->name)==0 || strcmp(network->source->via[j], "Null")==0) 
				i--;
			else 
			{
				data[i].connectToNeighbor = network->source->neighbors[i];
				data[i].attempts = attempts;
				data[i].sourceDV = network->source->dv;
				data[i].network  = network;
			}
			j++;
	}

	// Because we want the Neighbor's Data to be shared through all the threads, I don't want to allocate
	// memory to each one. So I allocate only to the first one, and connect the others to it with pointer.
	data[0].neighborsData = initializeNeighborData(network);
	if (data[0].neighborsData==NULL) {
		freeThreadData(data, network);
		return NULL;
	}
	for (i=1 ; i<numOfNeighbors ; i++) 
		data[i].neighborsData = data[0].neighborsData;
	
	return data;
}


void freeThreadData(ThreadData* data, Network* network)
{
	int i=0;

	if (data==NULL)
		return;

	for (i=0 ; i<network->source->numOfNeighbors+1 ; i++)
		free(data[0].neighborsData[i].sourceDV);
	free(data[0].neighborsData);
	free(data);
}


int createThreads(int attempts, Network* network)
{
	if (network==NULL || attempts<1) 
		return FALSE;

	int i;
	int numOfNeighbors = network->source->numOfNeighbors;
	pthread_t senders[numOfNeighbors];
	pthread_t receivers[numOfNeighbors];
	pthread_t calcs;
	ThreadData* data;

	data = initializeThreadData(attempts, network);
	if (data==NULL)
		return FALSE;

	// Creating 2 threads for each neighbor
	for (i=0 ; i<numOfNeighbors ; i++) 
	{
		if (pthread_create(&senders[i]  , NULL, (void*)sender  , (void*)&data[i]) != 0)
			return FALSE;
		if (pthread_create(&receivers[i], NULL, (void*)receiver, (void*)&data[i]) != 0)
		 	return FALSE;
	}
	// Creating calc thread.
	if (pthread_create(&calcs, NULL, (void*)calculator,(void*)data) != 0)
		return FALSE;

	// Join the threads.
	for (i=0 ; i<numOfNeighbors ; i++) 
	{
		if (pthread_join(senders[i], NULL) != 0)
			return FALSE;
		if (pthread_join(receivers[i], NULL) != 0)
			return FALSE;
	}
	if (pthread_join(calcs, NULL) != 0)
		return FALSE;
	
	freeThreadData(data, network);
	return 0;
}


void* sender(ThreadData* data)
{
	int sockfd,i=0;

	if (data==NULL)
        return NULL;
	
	sockfd = senderSocketConnect(data);
	while(1)
	{	
			pthread_mutex_lock(&mutex);
			write(sockfd, data->neighborsData[0].sourceDV,sizeof(int)*(data->network->numOfRouters+1));
			pthread_mutex_unlock(&mutex);
			if (finishFlag == 0)
			{
				close(sockfd); 
				return NULL;
			}
			pthread_mutex_lock(&mutex);

			for(i=0 ; i<data->network->source->numOfNeighbors ; i++)
				if (strcmp(data->network->source->neighbors[i], data->connectToNeighbor)==0)
					break;
		
			if(wait[i] == 0)
				pthread_cond_wait(&sendCondVar, &mutex);	
			wait[i]=0;
			pthread_mutex_unlock(&mutex);
	}
	close(sockfd);
	return NULL;
}


void* receiver(ThreadData* data)
{
	int sockfd, i, firstTimeFlag=0;
	int neighborDV[MAX_LENGTH];

	if (data==NULL)
		return NULL;
	
	sockfd = receiverSocketConnect(data);
	while(1)
	{		
		if (read(sockfd, neighborDV, MAX_LENGTH)==0) 
			if (finishFlag == 0) {
				close(sockfd);
				return NULL;
			}

		if (firstTimeFlag==0) {
			neighborDV[0] = 1;
			firstTimeFlag=FALSE;
		}
		if (finishFlag == 0)
		{
			close(sockfd); 
			return NULL;
		}
		
		pthread_mutex_lock(&mutex);
		updateNeighborDV(data, neighborDV);
		pthread_mutex_unlock(&mutex);

		memset(&neighborDV[0], 0, MAX_LENGTH);

		for(i=0 ; i<data->network->numOfRouters ; i++) {
			if(whoSentDV[i] == -1)
				break;
		}

		if(i==data->network->numOfRouters)
			pthread_cond_signal(&calcCondVar);
	}
	close(sockfd);
	return NULL;
}


void* calculator(ThreadData* data)
{
	int i, gotZeroCounter;

	while(1) 
	{
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&calcCondVar, &mutex);	
		gotZeroCounter=0;
		
		relax(data);

		for(i=0 ; i<data->network->numOfRouters ; i++)
			if(i != data->neighborsData[0].viaIndex && whoSentDV[i]==0)
				gotZeroCounter++;

		if(gotZeroCounter == data->network->source->numOfNeighbors) 
		{
			finishFlag = 0;
			for(i=0 ; i<data->network->numOfRouters; i++)
				printf("To: %s\tVia: %s\t%d\n", data->network->routers[i]->name, data->network->source->via[i], data->network->source->dv[i]);
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&sendCondVar);
			return NULL;
		}
		else
			initializeWhoSentDV(data->network);

		for(i=0 ; i<data->network->source->numOfNeighbors ; i++)
			wait[i] = 1;

		pthread_mutex_unlock(&mutex);
		pthread_cond_broadcast(&sendCondVar);
	}
	return NULL;
}


NeighborData* initializeNeighborData(Network* network)
{
	int i, j;
	NeighborData* neighborsData;
	
	neighborsData = (NeighborData*)malloc((network->source->numOfNeighbors+1)*sizeof(NeighborData));
	if(neighborsData==NULL)
		return NULL;

	//******************************************************************************
	//*******INITIALIZING the first "neighbor" which is the source at index 0*******
	//******************************************************************************
	neighborsData[0].name = network->source->name;
	neighborsData[0].sourceDV = (int*)malloc(sizeof(int)*(network->numOfRouters+1));
	if (neighborsData[0].sourceDV==NULL) {
		free (neighborsData);
		return NULL;
	}

	neighborsData[0].sourceDV[0] = 1;
	for (i=1 ; i<=network->numOfRouters ; i++) {
		neighborsData[0].sourceDV[i] = network->source->dv[i-1];
	}

	for (i=0 ; i<network->numOfRouters ; i++)
	{
		if(strcmp(network->source->via[i], network->source->name)==0) 
		{
			neighborsData[0].viaIndex = i;
			break;
		}		
	}

	//******************************************************************************
	//*****************INITIALIZING the rest of the neighbors***********************
	//******************************************************************************
	for (i=0 ; i<network->source->numOfNeighbors ; i++)
	{
		neighborsData[i+1].name = network->source->neighbors[i];
		neighborsData[i+1].sourceDV = (int*)malloc(sizeof(int)*(network->numOfRouters+1));
		if (neighborsData[i+1].sourceDV==NULL) {
			free (neighborsData);
			return NULL;
		}

		neighborsData[i+1].sourceDV[0] = 1;
		for (j=0 ; j<network->numOfRouters ; j++) {
			neighborsData[i+1].sourceDV[j+1] = INFINITI;
		}

		for (j=0 ; j<network->numOfRouters ; j++)
		{
			if(strcmp(neighborsData[i+1].name, network->source->via[j])==0) 
			{
				neighborsData[i+1].viaIndex = j;
				break;
			}		
		}
	}
	return neighborsData;
} 


void updateNeighborDV(ThreadData* data, int* neighborDV)
{
	int i,j;

	if(data==NULL || neighborDV==NULL)
		return;

	if (neighborDV[0] == 0) 
	{
		for (i=0 ; i<data->network->source->numOfNeighbors+1 ; i++) 
		{
			if (strcmp(data->connectToNeighbor, data->neighborsData[i].name)==0)
				whoSentDV[data->neighborsData[i].viaIndex] = 0;
		}
		return;
	}

	for (i=0 ; i<data->network->source->numOfNeighbors+1 ; i++)
	{
		if (strcmp(data->connectToNeighbor, data->neighborsData[i].name)==0)
		{
			for (j=0 ; j<data->network->numOfRouters+1 ; j++)
				data->neighborsData[i].sourceDV[j] = neighborDV[j];
			whoSentDV[data->neighborsData[i].viaIndex] = data->neighborsData[i].sourceDV[0];
			break;
		}
	}
}


void relax(ThreadData* data)
{
	int i, j, weight, changed=FALSE;

	for(i=1 ; i<data->network->source->numOfNeighbors+1 ; i++) 
	{
		// NO CHANGE WAS MADE
		if (data->neighborsData[i].sourceDV[0] == 0)
			continue;
		weight = data->network->source->dv[data->neighborsData[i].viaIndex];
		for(j=1 ; j<data->network->numOfRouters+1 ; j++)
		{
			if(data->neighborsData[0].sourceDV[j] > weight + data->neighborsData[i].sourceDV[j])
			{
				data->network->source->dv[j-1]     = weight + data->neighborsData[i].sourceDV[j];
				data->neighborsData[0].sourceDV[j] = weight + data->neighborsData[i].sourceDV[j];

				data->network->source->via[j-1] = data->neighborsData[i].name;
				changed=0;
			}
		}
	}
	if(changed == 0)
		data->neighborsData[0].sourceDV[0] = 1;
	else
		data->neighborsData[0].sourceDV[0] = 0;
}


void initializeWhoSentDV(Network* network)
{
	int i;

	for(i=0 ; i<network->numOfRouters ; i++)
	{
		if(areNeighboors(network, network->routers[i]) != FALSE)
			whoSentDV[i] = -1;
		else
			whoSentDV[i] = 1;
	}
}