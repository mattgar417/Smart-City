#ifndef RESOURCE_FLOW_GRAPH_H
#define RESOURCE_FLOW_GRAPH_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std;

// Graph structure for managing resource flow between districts.
class ResourceFlowGraph {
private:
    int nodes; // Number of nodes (districts) in the graph.
    map<int, vector<pair<int, double>>> adjList; // Adjacency list representing resource flows.

public:
    explicit ResourceFlowGraph(int n); // Constructor initializes a graph with n nodes.

    void addEdge(int from, int to, double weight); // Adds an edge between two nodes with a weight.
    void displayGraph();                           // Displays the adjacency list.

private:
    string getNodeLabel(int node);                 // Helper function to get labels for graph nodes.
};

#endif // RESOURCE_FLOW_GRAPH_H
