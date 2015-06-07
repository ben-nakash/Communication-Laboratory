#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>	
#include <netdb.h>

#define MAX_LENGTH 200
#define ROUTER_DEFINITION_LINE 3
#define INFINITI 10000000
#define FALSE -1


typedef struct router {
	char** via;
	char** neighbors;
	char* name;
	char* ip;
	int* dv;
	int port;
	int numOfNeighbors;
}Router;


typedef struct Edge {
	Router *source, *destination;
	int weight;
}Edge;


typedef struct Network
{
	Router* source;
	Router** routers;
	Edge** edges;
	int numOfEdges, numOfRouters;	
}Network;


typedef struct neighborData
{
	char* name;
	int viaIndex;
	int* sourceDV;
}NeighborData;


typedef struct threadData
{
	Network* network;
	char* connectToNeighbor;
	int* sourceDV;
	int attempts;
	NeighborData* neighborsData;
}ThreadData;



// This function is used to extract routers & edges from a given file, then by
// how many routers & edges it recognize, it creates a Network struct, that will 
// hold all of them in arrays. Returns the Network if success, or NULL if failed.
Network* readFromFile(FILE* file, char* sourceName);


// This function creates a Network, which is a struct that holds 2 lists:
// (1) For saving its routers ; (2) For saving its edges.
// Returns network pointer if success, and NULL if failed.
Network* createNetwork(int numOfEdges, int numOfRouters);


// This function creates a new router, which named with a given string.
// If success, it returns a pointer to the new router. If not it returns NULL.
Router*  createRouter(char* rName, char* rIP, int rPort);


// This function adds a given router to a given network. First it checks if its already
// exists. If it does, it frees that router and return a pointer to the already existed router.
// If it doesn't exist, it adds it to the network, and return a pointer to the router.
// If it fails for some reason it returns NULL.
Router*  addRouterToNetwork(Router* router, Network* network);


// This function search for a router with the given name inside the given network,
// and returns a pointer to it. If such not exists it returns null.
Router*  findRouterByName(char* name, Network* network);


// This function gets 2 routers & weight, and create a new edge which is added
// to the network if its not exist within it. Returns Edge if success, or NULL if failed.
Edge* createEdge(Router* src, Router* dst, int edgeWeight);


// This function initializing all the needed database, structs and variables in order
// to be able to create the threads later and do the calculations.
// Return 0 if succeeded, or -1(=FALSE) if failed.
int initialize(Network* network, char* argv);


// This function initializing the DV(=Distance Vector) of the source of the network.
// It allocates memory for it, and set it as follow:
// If its a neighbor - the distance between them. If its the source - 0, if neither - Infinity.
// Return 0 if succeeded, or -1(=FALSE) if failed.
int initializeDV(Network* network);


// This function initializing the VIA vector of the network's source.
// Return 0 if succeeded, or -1(=FALSE) if failed.
int initializeVIA(Network* network);


// Initializing all the needed data about the neighbors of the source.
// It creates an array of all the neighbors.
// Return 0 if succeeded, or -1(=FALSE) if failed.
int initializeNeighbors(Network* network);


// This function checks if a given router is a neighbor of the source.
// If it does - returns the weight of the edge connecting between them,
// If not, returns -1(=FALSE).
int areNeighboors(Network* network, Router* router);


// This function calls for another function to initialize all the needed database
// for the threads, and then create the threads and wait for them to finish.
// If an error occours, it returns -1(=FALSE). If finished successfully returns 0.
int createThreads(int attempts, Network* network);


// This function creates a sender socket connection.
// Return 0 if succeeded, or -1(=FALSE) if failed.
int senderSocketConnect(ThreadData* data);


// This function creates a receiver socket connection.
// Return 0 if succeeded, or -1(=FALSE) if failed.
int receiverSocketConnect(ThreadData* data);


// This function adds a given edge to a given network. First it checks if its already
// exists. If it does, it frees that edge. If it doesn't exist, it adds it to the network.
void addEdgeToNetwork(Edge* edge, Network* network);


// This function free the allocated memory of a given edge.
void freeEdge(Edge* edge);


// This function free the allocated memory of a given router.
void freeRouter(Router* router);


// This function free all the allocated memory of a whole network, which means
// it first free all the vertices & edges it holds, and only then free the network.
void freeNetwork(Network* network);


// This function checks the number of neighbors of a given router, and updates
// it within it's struct.
void checkNumOfNeighbors(Network* network, Router* router);


// This function free all the allocated memory of given ThreadData struct.
void freeThreadData(ThreadData* data, Network* network);


// Gets a DV of a neighbor and if needed (DV[0]!=0) it updates the the ThreadData
// with it and updates the array that saves data about who already sent his DV.
void updateNeighborDV(ThreadData* data, int* neighborDV);


// Relax function that works on the same principals as Bellman Ford algorithm.
void relax(ThreadData* data);

// Initializing the "WhoSentDV" array, allocate memory for it and set its values
// to be: -1 if the index represents a neighbor, else 1.
void initializeWhoSentDV(Network* network);


// This function sends the DV of the source via socket to a receiver of a neighbor.
void* sender(ThreadData* data);

// This function read a neighbors DV through a socket connection, and updates the
// neighbors data according to it.
void* receiver(ThreadData* data);

// This function waits until the source receive the DV from all it's neighbors, then
// it use the Relax function to recalculate the DV of the source.
void* calculator(ThreadData* data);

// This function allocates memory and initialize all needed parameters for the ThreadData
// struct, in order that each thread will have all the needed data.
// It returns a pointer to the allocated struct if succeed, or NULL of failed.
ThreadData* initializeThreadData(int attempts, Network* network);

// This function allocates memory and initializing all the needed data about neighbors.
// This data is part of the ThreadData.
// It returns a pointer to the allocated struct if succeed, or NULL of failed.
NeighborData* initializeNeighborData(Network* network);
