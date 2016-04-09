/**********************************************************************
cs2123p6.h
Purpose:
    Defines constants:
        max constants
        boolean constants
        error constants

    Defines typedef for
        Edge
        EdgeNode
        Vertex
        Graph
        GraphImp
 Protypes 
        PERT functions to be coded by students
        Aid printing the graph
        Building the graph
        Utility functions provided by Larry
Notes:

**********************************************************************/


/***  Constants ***/

// Maximum sizes
#define MAX_VERTICES 100
#define MAX_EDGES 100

// Error constants (program exit values)
#define ERR_COMMAND_LINE    900    // invalid command line argument
#define ERR_ALGORITHM       903    // Error in algorithm - almost anything else
#define ERR_BAD_INPUT       503    // Bad input 

// Error Messages 
#define ERR_MISSING_SWITCH          "missing switch"
#define ERR_EXPECTED_SWITCH         "expected switch, found"
#define ERR_MISSING_ARGUMENT        "missing argument for"

// exitUsage control 
#define USAGE_ONLY          0      // user only requested usage information
#define USAGE_ERR           -1     // usage error, show message and usage information

// boolean constants
#define FALSE 0
#define TRUE 1

// Represent the Graph using a double adjacency list
typedef struct Edge
{
    int iVertex;                    // If successor, this is TO vertex.  
                                    // If predecessor, this is FROM vertex.
    int iPath;                      // This is the weight of this arc.
} Edge;
// An edge node contains an edge and a next pointer
typedef struct EdgeNode
{
    Edge edge;                      // The edge
    struct EdgeNode *pNextEdge;     // Pointer to next edge
} EdgeNode;
// A vertex contains a label, information for PERT, and two pointers:
//    successor list and predecessor list.
typedef struct Vertex
{
    char cLabel;
    int iMaxFromSource;
    int iMaxFromSink;
    int iTL;
    EdgeNode * successorList;
    EdgeNode * predecessorList;
} Vertex;
// The Graph Implementation is an array of vertices with each vertex
// containing a pointer to a successor list and a predecessor list.
typedef struct
{
    int iNumVertices;
    Vertex vertexM[MAX_VERTICES];
} GraphImp;
typedef GraphImp *Graph;

// The struct Data type is used to store edges from the data before 
// building the graph using a double adjacency list.
struct Data
{
    char cFrom;
    char cTo;
    int iPath;
};

// prototypes for PERT functions that you must code
void computePertTEAndTL(Graph graph);
void freeLL(EdgeNode *edge);
void freeGraph(Graph graph);
void depthFirstPrintCriticalPaths(Graph g, int v, char szCriticalPath[], int iIndex);
void updateTL(Graph g, int maximumTE);
int findMaxTE(Graph g);
void computeMaxDistFromSource(Graph g, int v, int iPathLength);
void computeMaxDistFromSink(Graph g, int v, int iPathLength);
void printCriticalPaths(Graph g);
int findSinks(Graph g, int iSinkM[]);
int findSources(Graph g, int iSourceM[]);
void maxDistFromSource(Graph g);
void maxDistFromSink(Graph g);

// prototypes to aid printing
void printGraph(int iRun, char *pszSubject, Graph graph);
void printSpaces(char szSpaces[], int iRepeat);

// prototypes to get Graph Data and Build a Graph
int getGraphData(struct Data dataM[]);
Graph buildGraph(struct Data dataM[]);
Graph newGraph();
EdgeNode *allocateEdgeNode(Edge value);
EdgeNode *insertOrderedLL(EdgeNode **pHead, Edge value);
EdgeNode *searchLL(EdgeNode *pHead, int match, EdgeNode **ppPrecedes);
int searchVertices(Graph graph, char cVertex);

// functions in most programs, but require modifications
void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo);

// Utility routines provided by Larry (copy from program #2)
void ErrExit(int iexitRC, char szFmt[], ...);