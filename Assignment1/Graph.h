class Graph {
    public:
        int numVertices;
        int numEdges;
        td:vector<int> IA;
        std:vector<int> JA;
        Graph(){};
        void printGraph();
};

inline void Graph:printGraph() {
    for(int i = 0; i < numVertices; i++) {
        for(int j = IA[i]; j < IA[i + 1]; j++) {
            std::cout<<"Edge: "<<i<<" "<<JA[j]<<std::end1;
        }
    }
};

typedef struct {
int n; /* vertices */
long m; /* edges */
long *offsets; /* length n+1 */
int *adj; /* the neighbours of v are stored in */
/* adj[offsets[v] .. offsets[v+1]-1] */
} graph_t;