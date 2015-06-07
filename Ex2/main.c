#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ex2.c"

int main(int argc, char* argv[])
{
	int i, exist=-1;
	FILE* file;
	Graph* graph;
	Vertex* temp;

	// If the number of given arguments is not right, print an error.
	if (argc<3) {
		printf("ERROR - Missing arguments\n");
		return -1;
	}

	// Open file for read.
	file=fopen(argv[1], "r");
	if (file == NULL)
	{
		perror("Error while opening the file\n");
		exit(EXIT_FAILURE);
	}
	graph = fileRead(file);
	fclose(file);

	temp = createVertex(argv[2]);

	// Checking if such vertex exists in the graph, if not, throw an error and exit program.
	for (i=0 ; i<graph->numOfVertices ; i++) {
		if (strcmp(temp->data, graph->vertices[i]->data)==0)
				exist=0;
	}
	if (exist!=0) {
		printf("No such vertex, re-enter parameters.\n");
		exit(EXIT_FAILURE);
	}

	// The vertex exist, so I want to get the pointer to it.
	temp = addVertexToGraph(temp, graph);

	bellmanFord(graph, temp);

	printGraph(graph, temp);

	freeGraph(graph);	

	return 0;
}