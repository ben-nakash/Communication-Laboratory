#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ex2.h"


Graph* fileRead(FILE* file)
{
	char *token;
	char line[MAX_LENGTH], src[MAX_LENGTH], dst[MAX_LENGTH];
	int numOfVertex, edgeWeight, numOfEdges=0;
	Graph *graph;
	Vertex *newVer1, *newVer2;
	Edge *newEdge1, *newEdge2;

	// Checking how many edges we have.
	while (fgets(line, MAX_LENGTH, file) != NULL)
		numOfEdges++;
	numOfEdges = (numOfEdges-1)*2;	// numOfEdges = number of lines in file -1 (first line not included) * 2 
									// because an edge needed to be in both ways.
	rewind(file);	// Re-positioning to the begining of the file. 

	// Reading the first line
	if (fgets(line, MAX_LENGTH, file) != NULL)
	{
		token = strtok(line, " ");
		numOfVertex = atoi(token);
	}

	// Creating Graph struct
	graph = createGraph(numOfVertex, numOfEdges);
	if (graph == NULL) {
		printf("ERROR creating the Graph\n");
		return NULL;
	}

	// Continue reading the rest of the file, while creating, checking if exists and 
	// add new vertices and new edges to the graph.
	while (fgets(line, MAX_LENGTH, file) != NULL)
	{
		// Reading a line and put each parameter to a different temporary variable.
		token = strtok(line, " ");
		strcpy(src, token);
		token = strtok(NULL, " ");
		strcpy(dst, token);
		token = strtok(NULL, " ");
		edgeWeight = atoi(token);

		// Create 2 vertices.
		newVer1 = createVertex(src);
		newVer2 = createVertex(dst);
		
		// Checking if those vertices exists within the graph and adds them if needed.
		newVer1 = addVertexToGraph(newVer1, graph);
		newVer2 = addVertexToGraph(newVer2, graph);

		// Creating 2 edges, the given one and its vice versa.
		newEdge1 = createEdge(newVer1, newVer2, edgeWeight);
		newEdge2 = createEdge(newVer2, newVer1, edgeWeight);

		// Adding the edges to the graph.
		addEdgeToGraph(newEdge1, graph);
		addEdgeToGraph(newEdge2, graph);
	}
	return graph;
}


Vertex* createVertex(char* data)
{
	Vertex* newVertex;

	if (data==NULL)
		return NULL;
		
	newVertex = (Vertex*)malloc(sizeof(Vertex));
	if (newVertex==NULL)
		return NULL;

	newVertex->data = (char*)malloc(sizeof(data));
	if (newVertex->data == NULL) {
		freeVertex(newVertex);
		return NULL;
	}
	strcpy(newVertex->data, data);
	newVertex->distance = INFINITI;
	newVertex->pie = NULL;
	return newVertex;
} 


Edge* createEdge(Vertex* src, Vertex* dst, int weight)
{
	Edge* newEdge;

	if (src==NULL || dst==NULL)
		return NULL;
	
	newEdge = (Edge*)malloc(sizeof(Edge));
	if (newEdge == NULL)
		return NULL;
	newEdge->source = src;
	newEdge->destination = dst;
	newEdge->weight = weight;
	return newEdge;
}


Graph* createGraph(int numOfVertices, int numOfEdges)
{
	int i;
	Graph* graph;
	
	graph = (Graph*)malloc(sizeof(Graph));
	if (graph == NULL) 
		return NULL;
		
	// Allocating memory to the vertices list & the edges list.
	graph->vertices = (Vertex**)malloc(numOfVertices*sizeof(Vertex*));
	if (graph->vertices == NULL) {
		free(graph);
		return NULL;
	}
	graph->edges = (Edge**)malloc(numOfEdges*sizeof(Edge*));
	if (graph->edges == NULL) {
		free(graph->vertices);
		free(graph);
		return NULL;
	}
	
	// Going through those lists in order to avoid any junk that might be there.
	for (i=0 ; i<numOfVertices ; i++) {
		graph->vertices[i]=NULL;
	}
	for(i=0 ; i<numOfEdges ; i++) {
		graph->edges[i]=NULL;
	}
	graph->numOfEdges=numOfEdges;
	graph->numOfVertices=numOfVertices;
	return graph;
}


Vertex* addVertexToGraph(Vertex* vertex, Graph* graph)
{
	int i=0;
	if (vertex==NULL || graph==NULL) {
		printf("Error - NULL pointer in 'addVertexToGraph'\n");
		return NULL;
	}

	for (i=0 ; i<graph->numOfVertices ; i++)
	{
		// If nothing exists on this list index, add it.
		if (graph->vertices[i] == NULL) {
			graph->vertices[i] = vertex;
			return vertex;
		}
		// If the given vertex is already exist, it free the given one, and return the already existing one.
		if (graph->vertices[i] != NULL && strcmp(vertex->data, graph->vertices[i]->data)==0) {
			freeVertex(vertex);		
			vertex=NULL;
			return graph->vertices[i];	
		}
	}
	return NULL;	// If it got here there have been some kind of fault or error so it returns NULL.
}


void addEdgeToGraph(Edge* edge, Graph* graph)
{
	int i;
	if (edge==NULL || edge->source==NULL || edge->destination==NULL || graph==NULL) {
		printf("NULL pointer in 'addEdgeToGraph'\n");
		return;
	}
	
	for (i=0 ; i<graph->numOfEdges ; i++)
	{
		// Checking if such edge is already exist in the graph. If do, free this edge and finish.
		if(graph->edges[i]!=NULL && strcmp(edge->source->data, graph->edges[i]->source->data)==0 &&
			strcmp(edge->destination->data, graph->edges[i]->destination->data)==0)
		{
			freeEdge(edge);
			return;
		}
		// If it doesn't exist and there's nothing on that index location, add the edge to the graph.
		else if(graph->edges[i]==NULL) {
			graph->edges[i] = edge;
			return;	
		}
	}	
	
}							


void freeVertex(Vertex* vertex)
{
	if (vertex==NULL)
		return;
	if (vertex->data != NULL) {
		free(vertex->data);
		vertex->data=NULL;
	}
	free(vertex);
	vertex=NULL;
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


void freeGraph(Graph* graph)
{
	int i;
	if (graph==NULL)
		return;
	for (i=0 ; i<graph->numOfVertices ; i++) {
		freeVertex(graph->vertices[i]);
		graph->vertices[i]=NULL;
	}
	for (i=0 ; i<graph->numOfEdges ; i++) {
		freeEdge(graph->edges[i]);
		graph->edges[i]=NULL;
	}
	free(graph->vertices);
	graph->vertices=NULL;
	free(graph->edges);
	graph->edges=NULL;
	free(graph);
	graph=NULL;
}


void printGraph(Graph* graph, Vertex* source)
{
	int i;
	Vertex* temp;

	if (graph==NULL || source==NULL) {
		printf("ERROR - Null pointer.\n");
		return;
	}

	// Checking if there's a Negative weight cycle in the graph.
	for (i=0 ; i<graph->numOfVertices ; i++)
		if (graph->vertices[i]->distance < 0) {
			printf("A Negative weight cycle exists\n");
			break;
		}

	for (i=0 ; i<graph->numOfVertices ; i++) {
		//Printing dst.
		printf("%s\t", graph->vertices[i]->data);

		//Printing via.
		temp = graph->vertices[i];

		if (temp->distance == INFINITI || temp->distance==-INFINITI)	// If distance=Infinit it means this 
			printf("\t");												// vertex has no father.
		else {
			while (temp->pie != NULL && strcmp(temp->pie->data, source->data))
				temp = temp->pie;
			printf("%s\t", temp->data);
		}
		
		// Printing the distance.
		if (graph->vertices[i]->distance == INFINITI) {
			printf("∞\n");
			continue;
		}
		else if (graph->vertices[i]->distance == -INFINITI) {
			printf("-∞\n");
			continue;
		}
		printf("%d\n", graph->vertices[i]->distance);
	}
}


void bellmanFord(Graph* graph, Vertex* vertex)
{
	int i, j;
	vertex->distance=0;

	for (i=0 ; i<graph->numOfVertices ; i++) 
	{
		if (strcmp(vertex->data, graph->vertices[i]->data) != 0)
			graph->vertices[i]->distance=INFINITI;
	}

	for (i=0 ; i<graph->numOfVertices-1 ; i++)
	{
		for(j=0 ; j<graph->numOfEdges ; j++)
		{
			if (graph->edges[j]->destination->distance > graph->edges[j]->source->distance + graph->edges[j]->weight)
			{
				graph->edges[j]->destination->distance = graph->edges[j]->source->distance + graph->edges[j]->weight;
				graph->edges[j]->destination->pie = graph->edges[j]->source;
			}
		} 
	}

	for (i=0 ; i<graph->numOfEdges ; i++)
	{
		if (graph->edges[i]->destination->distance > graph->edges[i]->source->distance + graph->edges[i]->weight)
			graph->edges[i]->destination->distance = -INFINITI;
	}	
}









