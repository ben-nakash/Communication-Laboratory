#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LENGTH 40
#define INFINITI 10000000

typedef struct Vertex {
	char* data;
	struct Vertex* pie;
	int distance;
}Vertex;

typedef struct Edge {
	Vertex *source, *destination;
	int weight;
}Edge;

typedef struct Graph
{
	Vertex** vertices;
	Edge** edges;
	int numOfEdges, numOfVertices;	
}Graph;


// This function is used to extract vertices & edges from a given file, then by
// how many vertices & edges it recognize, it creates a Graph struct, that will 
// hold all of them in arrays. Returns the Graph if success, or NULL if failed.
Graph* fileRead(FILE* file);


// This function gets 2 vertices & weight, and create a new edge which is added
// to the graph if its not exist within it. Returns Edge if success, or NULL if failed.
Edge* createEdge(Vertex* src, Vertex* dst, int edgeWeight);


// This function creates a Graph, which is a struct that holds 2 lists:
// (1) For saving its vertices ; (2) For saving its edges.
// Returns Graph pointer if success, and NULL if failed.
Graph* createGraph(int numOfVertices, int numOfEdges);


// This function creates a new vertex, which named with a given string.
// If success, it returns a pointer to the new vertex. If not it returns NULL.
Vertex* createVertex(char* data);


// This function adds a given vertex to a given graph. First it checks if its already
// exists. If it does, it frees that vertex and return a pointer to the already existed vertex.
// If it doesn't exist, it adds it to the graph, and return a pointer to the vertex.
// If it fails for some reason it returns NULL.
Vertex* addVertexToGraph(Vertex* vertex, Graph* graph);


// This function adds a given edge to a given graph. First it checks if its already
// exists. If it does, it frees that edge. If it doesn't exist, it adds it to the graph.
void addEdgeToGraph(Edge* edge, Graph* graph);


// This function free the allocated memory of a given vertex.
void freeVertex(Vertex* vertex);


// This function free the allocated memory of a given edge.
void freeEdge(Edge* edge);


// This function free all the allocated memory of a whole graph, which means
// it first free all the vertices & edges it holds, and only then free the graph.
void freeGraph(Graph* graph);


// This function prints the needed output which is in the following format:
// dst via cost.
void printGraph(Graph* graph, Vertex* source);


// This function is as its name - the Bellman Ford algorithm. It computes shortest paths 
// from a single source vertex to all of the other vertices in a weighted graph.
void bellmanFord(Graph* graph, Vertex* vertex);













