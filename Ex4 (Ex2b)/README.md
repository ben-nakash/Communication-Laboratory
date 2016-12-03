Created by Ben Nakash as final exercise in Communication Laboratory course in Azrieli College of Engineering.

## Description
This exercise simulates a network of routers, when each of them runs a Bellman Ford algorithm. This algorithm's target is to find the
shortest route to all of the other routers within his reach. The exerciese was developed in linux environment. </br>
To simulate a network containing N routers, it is needed to run N terminals when each one of them receives a file with details about the
edges this router has and their weights. After all the routers have been initiallized this way, they calculate the shortest route to each
of the other routers within their reach. </br>
Each of the routers run 2 sockets on two different threads - one for reading information, and one for sending information. The protocol 
being used in this simulator is the TCP/IP protocol. In order to overcome issues like deadlocks - it required also a use in mutex, in 
order to lock critical code parts.

### Submited Files:
1. Makefile
2. README - Readme file with description of the contents & personal functions within the exercise.
3. ex2b.c - IMmplementation of the functions described in the header file.
4. ex2b.h - Definitions of the softwares structs, functions and constants. 
5. main.c

### Methods and Functions:
	1. readFromFile   -	Extracts routers & edges from a file and creates a Network to hold them. Return NULL if failed.
	2. createEdge 	  -	Gets 2 routers & weight, create an edge and adding it to a given network. Return NULL if failed.
	3. createNetwork  - 	Gets number of routers & edges and create a network struct. If failed return NULL.
	4. createRouter   -	Gets a name, ip & port and create a new Router. Return router if success or NULL if failed.
	5. addRouterToNetwork-	Adding a router to a network. If it already exist, it returns a pointer to the existed one.
	6. addEdgeToNetwork  -	Adding an edge to a network. If its already exists it frees the new one.
	7. freeRouter 		- 	Free the allocated memory of a given router.
	8. freeEdge 		-	Free the allocated memory of a given edge.
	9. freeNetwork 		- 	Free the allocated memory of a given network.
	10. findRouterByName    -	Search for a router with the given name within the network, and returns a pointer to it. If such not exists it returns null.
	11. initialize 		- 	Initializing all the needed database, structs and variables in order to be able to create the threads later and do the calculations. Return 0 if succeeded, or -1(=FALSE) if failed.
	12. initializeDV    -	This function initializing the DV(=Distance Vector) of the source of the network. Return 0 if succeeded, or -1(=FALSE) if failed.
	13. initializeVIA 	- 	This function initializing the VIA vector of the network's source. Return 0 if succeeded, or -1(=FALSE) if failed.
	14. initializeNeighbors  -	Initialize all the needed data about the neighbors of the source. Return 0 if succeeded, or -1(=FALSE) if failed.
	15. areNeighboors - Checks if a given router is a neighbor of the source. If it does - returns the weight of the edge connecting between them, If not, returns -1(=FALSE).
	16. createThreads - Initialize the needed database for the threads, creates them, and wait for them to finish. If an error occours, it returns -1(=FALSE). If finished successfully returns 0.
	17. senderSocketConnect   - Creates a sender socket connection. Return 0 if succeeded, or -1(=FALSE) if failed.
	18. receiverSocketConnect - Creates a receiver socket connection. Return 0 if succeeded, or -1(=FALSE) if failed.
	19. checkNumOfNeighbors   - Checks the number of neighbors of a given router, and updates it within it's struct.
	20. freeThreadData 	  - Free all the allocated memory of given ThreadData struct.
	21. updateNeighborDV - Gets a DV of a neighbor and updates the ThreadData if neccessary. Also updates the array that saves data about who already sent his DV.
	22. relax - Relax function that works on the same principals as Bellman Ford algorithm.
	23. initializeWhoSentDV - Initializing the "WhoSentDV" array, allocate memory for it and set its values to be: -1 if the index represents a neighbor, else 1.
	24. sender     - Sends the DV of the source via socket to a receiver of a neighbor.
	25. receiver   - Read a neighbors DV through a socket connection, and updates the neighbors data according to it.
	26. calculator - Waits until the source receive the DV from all it's neighbors, then it use the Relax function to recalculate the DV of the source.
	27. initializeThreadData - Allocates memory and initialize all needed parameters for the ThreadData struct, in order that each thread will have all the needed data. returns a pointer to the allocated struct if succeed, or NULL of failed.
	28. initializeNeighborData - Allocates memory and initializing all the needed data about neighbors. This data is part of the ThreadData. It returns a pointer to the allocated struct if succeed, or NULL of failed.
