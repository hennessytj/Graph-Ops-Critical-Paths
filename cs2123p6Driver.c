/**********************************************************************
cs2123p6Driver.c by Timothy Hennessy
Purpose:	
   Reads input from a redirected file, builds a graph and determines
   the critical paths in accordance with PERT (Program Evaluation 
   and Review Technique) standards.
Command Parameters:
   There are no command parameters.  This program uses files redirected
   from stdin.  
Input:
   Data will contain multiple sets of edges (one set for each graph)
   From To PathWeight
   %c   %c    %d
   Each set of edges is terminated by a record having 0 0 0
   Example input:
   A B 5
   A E 7
   B C 4
   C D 8
   E C 5
   E F 6
   F D 4
   F G 2
   H E 11
   H I 6
   I G 9
   0 0 0 
Results:
   Graph # 3: PERT TE and TL
   Vtx TE TL	SUCCESSORS                  PREDECESSORS            
   Q    0  0	A  6  D  8                  -
   A    6  6	B  5  D  3  C 11            Q  6 
   B   11 27	T  6                        A  5 
   T   27 33	-                           B  6 F  7 
   D    9 13	C  4                        Q  8 A  3 
   C   17 17	F  3                        A 11 D  4 
   F   20 20	T  7  G  8                  C  3 E  7 
   R    0  0	E  7  H 10                  -
   E    7 11	F  7  I  7                  R  7 
   G   28 28	U  5                        F  8 
   U   33 33	-                           G  5 I 15 
   I   18 18	U 15  J  2  K  7            E  7 H  8 
   H   10 10	I  8  J  6                  R 10 S  6 
   S    0  4	H  6                        -
   J   20 21	K  4                        I  2 H  6 
   K   25 25	V  8                        I  7 J  4 
   V   33 33	-                           K  8 
   Critical Path(s)
   Q A C F G U
   R H I U
   R H I K V
Returns:
   Program prints out to stdout and does not return anything.
Notes:
    Function uses a double adjancency list to store graph.  PERT
    (Program Evaluation and Review Techniques) requires the TE
    (the earliest time to completion along each path from source
    to sink), the maximum TE, and TL (TL = TE - maximum distance 
    from the sink for current vertex).
**********************************************************************/

#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "cs2123p6.h"
/********************************** freeLL ***********************************************
void freeLL(EdgeNode *edge)
Purpose:
    Recursively traverses linked list and frees each node.
Parameters:
    I  EdgeNode *edge                  Pointer to an EdgeNode
Notes:
    Recursively traverses linked list of EdgeNodes until there are no more (i.e., NULL).
    Once NULL, the function will return and free each EdgeNode in the linked list. 
Returns:
    N/A
*******************************************************************************************/
void freeLL(EdgeNode *edge)
{
	if (edge == NULL)
		return;
	freeLL(edge->pNextEdge);
	free(edge);
}
/********************************** freeGraph *******************************************
void freeGraph(Graph g)
Purpose:
    Iteratively goes through vertexM[] freeing each successorList and predecessorList
    linked list.  
Parameters:
    I  Graph g                  Pointer to a graphImp
Notes:
    For each vertexM it passes a pointer to an EdgeNode to freeLL which frees both of 
    the current vertexM's linked lists (successorList and predecessorList).  When 
    finished each linked list is set to NULL to prevent a dangling reference.  
    Lastly, it frees the graph (i.e., the entire vertexM[]). 
Returns:
    N/A
*******************************************************************************************/
void freeGraph(Graph g)
{
	int i;     // index for vertexM 
	for (i = 0; i < g->iNumVertices; i++)
	{
		freeLL(g->vertexM[i].successorList);
		g->vertexM[i].successorList = NULL;
		freeLL(g->vertexM[i].predecessorList);	
		g->vertexM[i].predecessorList = NULL;
	}
	free(g);
}
/************************** depthFirstPrintCriticalPaths ***********************************
void depthFirstPrintCriticalPaths(Graph g, int v, char szCriticalPath[], int iIndex)
Purpose:
    Prints the critical path(s) of graph.
Parameters:
    I  Graph g                  Pointer to a graphImp
    I  int v                    Index for vertedM[]
    O  char szCriticalPath[]    Stores critical paths
    I  int iIndex               Index of szCriticalPath[]
Notes:
    Called by printCriticalPaths to traverse double adjacency list searching for critical
    path(s).  Uses a depth first traversal.  If function traverses to a vertex that does
    not have equivalent values of iMaxFromSource and iTL it will stop traversing that path.
    When a sink is found the contents of the szCriticalPath array are printed along with 
    the sink, the function then returns back to check for other critical paths.    
Returns:
    N/A
*******************************************************************************************/
void depthFirstPrintCriticalPaths(Graph g, int v, char szCriticalPath[], int iIndex)
{
	EdgeNode *e;
	if (g->vertexM[v].iMaxFromSource != g->vertexM[v].iTL)
		return;
	if (g->vertexM[v].successorList == NULL)
	{
		printf("%s", szCriticalPath);
		printf("%c\n", g->vertexM[v].cLabel);
		return;
	}
	szCriticalPath[iIndex++] = g->vertexM[v].cLabel;   // add char to string and increment index
	szCriticalPath[iIndex++] = ' ';                    // include space after char and increment index
	szCriticalPath[iIndex] = '\0';                     // add NULL byte
	for (e = g->vertexM[v].successorList; e != NULL; e = e->pNextEdge)
	{
	 depthFirstPrintCriticalPaths(g, e->edge.iVertex, szCriticalPath, iIndex);	
	}
}
/************************** printCriticalPaths ***********************************
void printCriticalPaths(Graph g)
Purpose:
    Prints the critical path(s) of graph.
Parameters:
    I  Graph g              Pointer to a graphImp
Notes:
    Calls depthFirstPrintCriticalPaths for each source in graph that is part of
    a critical path.  
Returns:
    N/A
**********************************************************************************/
void printCriticalPaths(Graph g)
{
	int iNumSources;            // stores total number of
	                            // sources in graph
	int i;                      // index of vertexM array
	int iSourceM[MAX_VERTICES];
	char szCriticalPath[MAX_VERTICES];
	iNumSources = findSources(g, iSourceM);
	if (iNumSources <= 0)
		ErrExit(ERR_ALGORITHM, "number of sources returned is 0");
	printf("Critical Path(s)\n");
	for (i = 0; i < iNumSources; i++)
	{ 
		if (g->vertexM[iSourceM[i]].iMaxFromSource == g->vertexM[iSourceM[i]].iTL)
		    depthFirstPrintCriticalPaths(g, iSourceM[i], szCriticalPath, 0);
	}
}
/************************** updateTL ****************************************
void updateTL(Graph g, int maximumTE)
Purpose:
    Computes the TL for each vertex in vertexM[].
Parameters:
    I  Graph g              Pointer to a graphImp
    I  int maximumTE        Integer value of largest TE in graph
Notes:
    TE is calculated by subtracting the maximum TE from the maximum distance
    from sink.
Returns:
    N/A
**********************************************************************************/
void updateTL(Graph g, int maximumTE)
{
	int i;            // index of vertexM array
	for (i = 0; i < g->iNumVertices; i++)
	{
		g->vertexM[i].iTL = maximumTE - g->vertexM[i].iMaxFromSink;
	}
}
/************************** findMaxTE ****************************************
int findMaxTE(Graph g)
Purpose:
    Searches for maximum TE in graph.
Parameters:
    I  Graph g              Pointer to a graphImp
Notes:
    In the worst case function does 'n' comparisons when n is equal to 
    the number of vertices in graph (i.e., n = g->vertexM[v].iNumVertices)
Returns:
    Returns the largest TE in graph.
**********************************************************************************/
int findMaxTE(Graph g)
{
	int i;                    // index of vertexM array
	int max = -9999;          // will store the largest
	                          // iMaxFromSource value from graph
	for (i = 0; i < g->iNumVertices; i++)
	{
		if (g->vertexM[i].iMaxFromSource > max)
			max = g->vertexM[i].iMaxFromSource;
	}
	return max;
}
/******************** computeMaxDistFromSink ****************************************
void computeMaxDistFromSink(Graph g, int v, int iPathLength)
Purpose:
    Takes a source from a graph and uses a depth first traversal to 
    compute the maximum distance of each vertex from each sink.
Parameters:
    I  Graph g              Pointer to a graphImp
    I  int v                Integer value corresponding to a vertex
    I  int iPathLength      Integer value that stores current path length
                            between nodes
Notes:
    This function recursively traverses the graph using a depth first
    traversal approach.  It will update iMaxfromSource for each vertex
    in the graph.  Will visit a node and then using a for loop traverse to the
    next successor and continue this pattern until NULL.  If the value of the
    successors iMaxFromSource is less than the current nodes iMaxFromSource +
    the successors iPath (distance) then the successors iMaxFromSource is 
    changed to the higher value.
Returns:
    N/A
**********************************************************************************/
void computeMaxDistFromSink(Graph g, int v, int iPathLength)
{
	EdgeNode *e;
	for (e = g->vertexM[v].predecessorList; e != NULL; e = e->pNextEdge)
	{
		if (g->vertexM[e->edge.iVertex].iMaxFromSink < (g->vertexM[v].iMaxFromSink + e->edge.iPath))
			g->vertexM[e->edge.iVertex].iMaxFromSink = g->vertexM[v].iMaxFromSink + e->edge.iPath;
		computeMaxDistFromSink(g, e->edge.iVertex, e->edge.iPath);
	}
}
/******************** findSinks **************************************
int findSources(Graph g, int iSinkM[])
Purpose:
    Searches through graph for the sink vertices. 
Parameters:
    I  Graph g            Pointer to a graphImp
    O  int iSinkM[]       Array of integers used to hold index of sink
                          vertices
Notes:
    Each graph contains an array of Vertex structures.  This function
    iteratively searches through this array seeking vertices that do not
    have successors.  This function assumes that vertex without successors 
    is a sink vertex.  After iNumVertices - 1 iterations, the integer value
    'j' is returned to the calling function.  If j = 0, something is not right.
Returns:
    Functionally:
           j            - integer of total number of sinks in graph
    Via parameters:
           iSinkM       - array that stores indexes of sink vertices 
                          from graph
**************************************************************************/
int findSinks(Graph g, int iSinkM[])
{
	int i;                 // index used by for loop
	int j = 0;             // index used for iSinkM       
	for (i = 0; i < g->iNumVertices; i++)
	{
		if (g->vertexM[i].successorList == NULL)
		{  
			iSinkM[j] = i;    // assign source indexes
			j++;
		}
	} // end for
	return j; // should be total number of sinks in graph
}
/******************** maxDistFromSink **************************************
void maxDistFromSink(Graph g)
Purpose:
    Computes the maximum distances from sinks in graph. 
Parameters:
    I  Graph g              Pointer to a graphImp
Notes:
    This function calls findSinks which returns the number of sinks and
    an array of indexes corresponding to the sink vertices.  This function
    then calls a function which computes the maximum distances from each 
    sink to each vertex in the graph.  If the current maximum distance is
    greater than the new maximum distance, current maximum distance is
    kept and the traversal along this path stops.  If the current maximum
    distance is less than the new maximum distance, the new maximum distance
    replaces the old maximum distance and the path is traversed until NULL or
    a current maximum distance is greater.
Returns:
    This function does not return anything but it does update the each 
    iMaxFromSink distance for each vertex in the graph.
**************************************************************************/
void maxDistFromSink(Graph g)
{
	int iSinkM[MAX_VERTICES];                        // used to store indexes of sources
	int iNumSinks;                                   // number of sources found in g
	int i;
	memset(iSinkM, -1, MAX_VERTICES*sizeof(int));    // initialize array to -1 
	                                                 //  -1 indicates that current array element is not source
	iNumSinks = findSinks(g, iSinkM);
	if (iNumSinks <= 0)
		ErrExit(ERR_ALGORITHM, "number of sinks returned is 0");
    for (i = 0; i < iNumSinks; i++)
	{   // initial call starts with sources, path length must be 0
		computeMaxDistFromSink(g, iSinkM[i], 0);
	}
}
/******************** computeMaxDistFromSource **************************************
void computeMaxDistFromSource(Graph g, int v, int iPathLength)
Purpose:
    Takes a source from a graph and uses a depth first traversal to 
    compute the maximum distance of each vertex from each source.
Parameters:
    I  Graph g              Pointer to a graphImp
    I  int v                Integer value corresponding to a vertex
    I  int iPathLength      Integer value that stores current path length
                            between nodes
Notes:
    This function recursively traverses the graph using a depth first
    traversal approach.  It will update iMaxfromSource for each vertex
    in the graph.  Will visit a node and then using a for loop traverse to the
    next successor and continue this pattern until NULL.  If the value of the
    successors iMaxFromSource is less than the current nodes iMaxFromSource +
    the successors iPath (distance) then the successors iMaxFromSource is 
    changed to the higher value.
Returns:
    N/A
**********************************************************************************/
void computeMaxDistFromSource(Graph g, int v, int iPathLength)
{
	EdgeNode *e;
	for (e = g->vertexM[v].successorList; e != NULL; e = e->pNextEdge)
	{
		if (g->vertexM[e->edge.iVertex].iMaxFromSource < (g->vertexM[v].iMaxFromSource + e->edge.iPath))
			g->vertexM[e->edge.iVertex].iMaxFromSource = g->vertexM[v].iMaxFromSource + e->edge.iPath;
		computeMaxDistFromSource(g, e->edge.iVertex, e->edge.iPath);
	}
}
/******************** findSources **************************************
int findSources(Graph g, int iSourceM[])
Purpose:
    Searches through graph for the source vertices. 
Parameters:
    I  Graph g              Pointer to a graphImp
    O  int iSourceM[]       Array of integers used to hold index of source
                            vertices
Notes:
    Each graph contains an array of Vertex structures.  This function
    iteratively searches through this array seeking vertices that do not
    have predecessors.  This function assumes that vertex without predecessors 
    is a source vertex.  After iNumVertices - 1 iterations, the integer value
    'j' is returned to the calling function.  If j = 0, something is not right.
Returns:
    Functionally:
           j            - integer of total number of sources in graph
    Via parameters:
           iSourceM     - array that stores indexes of source vertices 
                          from graph
**************************************************************************/
int findSources(Graph g, int iSourceM[])
{
	int i;                 // index used by for loop
	int j = 0;             // index used for iSourceM       
	for (i = 0; i < g->iNumVertices; i++)
	{
		if (g->vertexM[i].predecessorList == NULL)
		{  
			iSourceM[j] = i;    // assign source indexes
			j++;
		}
	} // end for
	return j; // should be total number of sources in graph
}
/******************** maxDistFromSource **************************************
void maxDistFromSource(Graph g)
Purpose:
    Computes the maximum distances from sources in graph. 
Parameters:
    I  Graph g              Pointer to a graphImp
Notes:
    This program calls findSources to locate each source in graph (i.e.,
    vertices without any predecessors).  The function findSources returns
    the number of sources found and through a parameter an array of indexes
    corresponding to these sources.  After the sources have been identified
    this function uses a for loop to send each source into a depth first 
    traversal function to calculate the maximum distances from each source
    to each vertex.
Returns:
    This function updates the graph's vertices with maximum distances from
    the source vertices.  Aside from this the function does not return
    anything (well, maybe some bugs:-D).
**************************************************************************/
void maxDistFromSource(Graph g)
{
	int iSourceM[MAX_VERTICES];                        // used to store indexes of sources
	int iNumSources;                                   // number of sources found in g
	int i;
	iNumSources = findSources(g, iSourceM);
	if (iNumSources <= 0)
		ErrExit(ERR_ALGORITHM, "number of sources returned is 0");
    for (i = 0; i < iNumSources; i++)
	{   // initial call starts with sources, path length must be 0
		computeMaxDistFromSource(g, iSourceM[i], 0);
	}
}
void computePertTEAndTL(Graph graph)
{
	int maxTE;              // store largest iMaxFromSource 
	                        // (i.e., max TE) value found in graph
	maxDistFromSource(graph);
	maxDistFromSink(graph);
    maxTE = findMaxTE(graph);
    updateTL(graph, maxTE);
}
/* end of p6 student written functions */
// dataM has the edges from the data
struct Data dataM[MAX_EDGES];

int main()
{
    int iRun = 0;
    Graph graph;
    while (getGraphData(dataM) == TRUE)
    {
        // Build the graph from the dataM array
        graph = buildGraph(dataM);
        iRun++;
        printGraph(iRun, "Initial", graph);

        // Compute TE and TL
        computePertTEAndTL(graph);
        printGraph(iRun, "PERT TE and TL", graph);

        // Print critical paths
        printCriticalPaths(graph);
        printf("\n");

        // free the graph
        freeGraph(graph);
    }
    return 0;
}
/******************** getGraphData **************************************
  int getGraphData(struct Data dataM[])
Purpose:
    Populates an array of edges with data from stdin.  The array is 
    terminated when EOF or a 0 0 0 is encountered in the data. 
Parameters:
    O   struct Data dataM[]     An array of edges (to, from, path weight)
Notes:
    - The array of edges is terminated by an edge having a from vertex
      equal to the character '0'.
Returns:
    TRUE - data was read to populate the array of vertices.
    FALSE - no data found.
**************************************************************************/
int getGraphData(struct Data dataM[])
{
    char szInput[100];
    int i = 0;
    int iScanfCnt;
    while (fgets(szInput, 50, stdin) != NULL)
    {
        if (i > MAX_EDGES)
            ErrExit(ERR_BAD_INPUT, "too many edges");
        iScanfCnt = sscanf(szInput, "%c %c %d"
            , &dataM[i].cFrom, &dataM[i].cTo, &dataM[i].iPath);
        if (iScanfCnt < 3)
            ErrExit(ERR_BAD_INPUT, " Found: '%s', scanf count is %d", szInput, iScanfCnt);
        if (dataM[i].cFrom == '0')
        {
            dataM[i].cFrom = '\0';
            break;
        }
        i++;
    }
    return i > 0;
}
/******************** newGraph **************************************
    Graph newGraph()
Purpose:
    Allocates a GraphImp and initializes its data.  It returns a
    Graph. 
Parameters:
    n/a
Notes:
    - Uses malloc to allocate a GraphImp.  It also makes certain
      that malloc didn't fail.
    - Initializes iNumvetices to 0.
    - Initializes the array of vertices to zero bytes.
    - 
Returns:
    Graph - the newly allocated graph.
**************************************************************************/
Graph newGraph()
{
    Graph g = (Graph)malloc(sizeof(GraphImp));
    if (g == NULL)
        ErrExit(ERR_ALGORITHM, "malloc for Graph failed");
    g->iNumVertices = 0;
    memset(g->vertexM, '\0',  sizeof(Vertex)*MAX_VERTICES);
    return g;
}
/******************** buildGraph **************************************
    Graph buildGraph(struct Data dataM[])
Purpose:
    Builds a graph from an array of edges.  The Graph is represented by
    an array of vertices and a double adjacency list.
Parameters:
    I   struct Data dataM[]     An array of edges (to, from, path weight)

Notes:
    - The array of edges is terminated by an edge having a from vertex
      equal to the character '0'.
    - Normally, this routine checks to make certain that the input did not
      include a cycle, but since that code wasn't included, the check
      has been commented out.
Returns:
    Graph - the newly allocated and populated graph.
**************************************************************************/
Graph buildGraph(struct Data dataM[])
{
    int i;
    int iFrom;
    int iTo;
    Edge edge;
    Graph graph = newGraph();
    // Initialize the vertex array to zero bytes.
    memset(graph->vertexM, '\0', sizeof(Vertex)*MAX_VERTICES);

    // Go through the array of edges until a From of '\0' is encountered
    for (i = 0; i < MAX_VERTICES; i++)
    {
        if (dataM[i].cFrom == '\0')
            break;
        // Find the From vertex.  searchVertices returns -1 for not found
        iFrom = searchVertices(graph, dataM[i].cFrom);
        if (iFrom == -1)
        {   // not found, so insert the From vertex in the array
            iFrom = graph->iNumVertices;
            graph->iNumVertices++;
            graph->vertexM[iFrom].cLabel = dataM[i].cFrom;
        }
        // Find the To vertex.  searchVertices returns -1 for not found
        iTo = searchVertices(graph, dataM[i].cTo);
        if (iTo == -1)
        {   // not found, so insert the To vertex in the array
            iTo = graph->iNumVertices;
            graph->iNumVertices++;
            graph->vertexM[iTo].cLabel = dataM[i].cTo;
        }
        // insert the edge on both the successor and predecessor lists
        edge.iPath = dataM[i].iPath;
        edge.iVertex = iTo;
        insertOrderedLL(&graph->vertexM[iFrom].successorList, edge);
        edge.iVertex = iFrom;
        insertOrderedLL(&graph->vertexM[iTo].predecessorList, edge);
    }
//    if (!checkCycle(graph))
//        ErrExit(ERR_BAD_INPUT, "Cycle in Graph");
    return graph;
}
/******************** searchVertices **************************************
   int searchVertices(Graph graph, char cVertex)
Purpose:
    Searches the graph's array of vertices for the specified vertex.  If 
    found, it returns the subscript.  If not found, it returns -1.
Parameters:
    I   Graph graph             A directed graph represented by a double
                                adjacency list an an array of vertices.
    I   char cVertex            The vertex's label to be found.
Notes:
    - this just uses a linear search.  It can be greatly improved using
      a binary search (if sorted) or hashing
Returns:
    >= 0 - subscript in the array of vertices if the vertex label is found 
    -1 - not found
**************************************************************************/
int searchVertices(Graph graph, char cVertex)
{
    int i;
    for (i = 0; i < graph->iNumVertices; i++)
    {
        if (cVertex == graph->vertexM[i].cLabel)
            return i;
    }
    return -1;
}
/******************** printGraph **************************************
   void printGraph(int iRun, char *pszSubject, Graph graph)
Purpose:
    Prints a graph which is represensted by a double adjacency list.
    When printing the information about a vertex, it prints the PERT
    TE and TL.  For each vertex, it prints:
        vertex label
        TE
        TL
        predecessors
        successors
Parameters:
    I   int iRun                Identifies which graph is being displayed. 
    I   char *pszSubject        A message to display about this graph.
    I   Graph graph             A directed graph represented by a double
                                adjacency list.  
Notes:
    - Since predecessors and successors are printed on the same line,
      it uses the function printSpaces to help have all the lists of
      predecessors appear in the same column.
Returns:
    n/a
**************************************************************************/
void printGraph(int iRun, char *pszSubject, Graph graph)
{
    int iv;
    EdgeNode *e;
    int iCnt;
    // Print headings
    printf("Graph #%2d: %s\n", iRun, pszSubject);
    printf("%3s %2s %2s\t%-24s    %-24s\n"
        , "Vtx", "TE", "TL", "SUCCESSORS", "PREDECESSORS");
    // for each vertex in the graph
    for (iv = 0; iv < graph->iNumVertices; iv++)
    {
        // print the vertex label, TE, and TL
        printf("%c   %2d %2d\t", graph->vertexM[iv].cLabel
            , graph->vertexM[iv].iMaxFromSource
            , graph->vertexM[iv].iTL);
        iCnt = 0;
        // print each of its successors
        for (e = graph->vertexM[iv].successorList; e != NULL; e = e->pNextEdge)
        {
            iCnt++;
            printf("%c %2d  "
                , graph->vertexM[e->edge.iVertex].cLabel
                , e->edge.iPath);
        }
        // if it didn't have successors, print a "-"
        if (iCnt == 0)
        {
            printf("-     ");
            iCnt = 1;
        }
        // determine the variable-spaced amount for including spaces to get the 
        // predecessor list to print in a specific column
        printSpaces("      ", 4 - iCnt);
        printf("    ");
        iCnt = 0;
        // Print each of its predecessors
        for (e = graph->vertexM[iv].predecessorList; e != NULL; e = e->pNextEdge)
        {
            printf("%c %2d "
                , graph->vertexM[e->edge.iVertex].cLabel
                , e->edge.iPath);
            iCnt++;
        }
        // if it didn't have predecessors, print a "-"
        if (iCnt == 0)
            printf("-");
        printf("\n");
    }
}
/******************** printSpaces **************************************
void printSpaces(char szSpaces[], int iRepeat)
Purpose:
    Prints spaces across a line of output to help with consistent columns.
Parameters:
    I   char szSpaces           A string of spaces to be printed
    I   iRepeat                 The number of times to print that string
                                of spaces.   
Returns:
    n/a
**************************************************************************/
void printSpaces(char szSpaces[], int iRepeat)
{
    int i;
    for (i = 0; i < iRepeat; i++)
    {
        printf("%s", szSpaces);
    }
}

// Linked list manipulation routines used for successor and predecessor lists
// These routines come from the course notes, but were changed slightly 
// for the particular Node and element structures
EdgeNode *searchLL(EdgeNode *pHead, int match, EdgeNode **ppPrecedes)
{
    EdgeNode *p;
    // used when the list is empty or we need to insert at the beginning
    *ppPrecedes = NULL;

    // Traverse through the list looking for where the key belongs or
    // the end of the list.
    for (p = pHead; p != NULL; p = p->pNextEdge)
    {
        if (match == p->edge.iVertex)
            return p;
        if (match < p->edge.iVertex)
            return NULL;
        *ppPrecedes = p;
    }

    // Not found return NULL
    return NULL;
}

EdgeNode *insertOrderedLL(EdgeNode **pHead, Edge value)
{
    EdgeNode *pNew, *pFind, *pPrecedes;
    // see if it already exists
    pFind = searchLL(*pHead, value.iVertex, &pPrecedes);
    if (pFind != NULL)
        return pFind;

    // doesn't already exist.  Allocate a node and insert.
    pNew = allocateEdgeNode(value);

    // Check for inserting at the beginning of the list
    // this will also handle when the list is empty
    if (pPrecedes == NULL)
    {
        pNew->pNextEdge = *pHead;
        *pHead = pNew;
    }
    else
    {
        pNew->pNextEdge = pPrecedes->pNextEdge;
        pPrecedes->pNextEdge = pNew;
    }
    return pNew;
}

EdgeNode *allocateEdgeNode(Edge value)
{
    EdgeNode *pNew;
    pNew = (EdgeNode *)malloc(sizeof(EdgeNode));
    if (pNew == NULL)
        ErrExit(ERR_ALGORITHM, "No available memory for linked list");
    pNew->edge = value;
    pNew->pNextEdge = NULL;
    return pNew;
}

/******************** ErrExit **************************************
  void ErrExit(int iexitRC, char szFmt[], ... )
Purpose:
    Prints an error message defined by the printf-like szFmt and the
    corresponding arguments to that function.  The number of 
    arguments after szFmt varies dependent on the format codes in
    szFmt.  
    It also exits the program with the specified exit return code.
Parameters:
    I   int iexitRC             Exit return code for the program
    I   char szFmt[]            This contains the message to be printed
                                and format codes (just like printf) for 
                                values that we want to print.
    I   ...                     A variable-number of additional arguments
                                which correspond to what is needed
                                by the format codes in szFmt. 
Notes:
    - Prints "ERROR: " followed by the formatted error message specified 
      in szFmt. 
    - Prints the file path and file name of the program having the error.
      This is the file that contains this routine.
    - Requires including <stdarg.h>
Returns:
    Returns a program exit return code:  the value of iexitRC.
**************************************************************************/
void ErrExit(int iexitRC, char szFmt[], ...)
{
    va_list args;               // This is the standard C variable argument list type
    va_start(args, szFmt);      // This tells the compiler where the variable arguments
    // begins.  They begin after szFmt.
    printf("ERROR: ");
    vprintf(szFmt, args);       // vprintf receives a printf format string and  a
    // va_list argument
    va_end(args);               // let the C environment know we are finished with the
    // va_list argument
    printf("\n");
    exit(iexitRC);
}
/******************** exitUsage *****************************
    void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo)
Purpose:
    In general, this routine optionally prints error messages and diagnostics.
    It also prints usage information.

    If this is an argument error (iArg >= 0), it prints a formatted message 
    showing which argument was in error, the specified message, and
    supplemental diagnostic information.  It also shows the usage. It exits 
    with ERR_COMMAND_LINE.

    If this is a usage error (but not specific to the argument), it prints 
    the specific message and its supplemental diagnostic information.  It 
    also shows the usage and exist with ERR_COMMAND_LINE. 

    If this is just asking for usage (iArg will be -1), the usage is shown.
    It exits with USAGE_ONLY.
Parameters:
    I int iArg                      command argument subscript or control:
                                    > 0 - command argument subscript
                                    0 - USAGE_ONLY - show usage only
                                    -1 - USAGE_ERR - show message and usage
    I char *pszMessage              error message to print
    I char *pszDiagnosticInfo       supplemental diagnostic information
Notes:
    This routine causes the program to exit.
**************************************************************************/
void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo)
{
    switch (iArg)
    {
    case USAGE_ERR:
        fprintf(stderr, "Error: %s %s\n"
            , pszMessage
            , pszDiagnosticInfo);
        break;
    case USAGE_ONLY:
        break;
    default:
        fprintf(stderr, "Error: bad argument #%d.  %s %s\n"
            , iArg
            , pszMessage
            , pszDiagnosticInfo);
    }
    // print the usage information for any type of command line error
    fprintf(stderr, "p6 \n");
    if (iArg == USAGE_ONLY)
        exit(USAGE_ONLY);
    else
        exit(ERR_COMMAND_LINE);
}