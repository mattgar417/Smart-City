#include "ResourceFlowGraph.h"

// Constructor initializes a graph with a given number of nodes.
ResourceFlowGraph::ResourceFlowGraph(int n) : nodes(n) {}

// Adds an edge from one node to another with a weight.
void ResourceFlowGraph::addEdge(int from, int to, double weight) {
    adjList[from].push_back({to, weight});
}

// Displays the adjacency list of the graph.
void ResourceFlowGraph::displayGraph() {
    cout << "\nResource Flow Graph:\n";
    for (const auto& [node, edges] : adjList) {
        cout << getNodeLabel(node) << ": ";
        for (const auto& [neighbor, weight] : edges) {
            cout << "(" << getNodeLabel(neighbor) << ", " << weight << ") ";
        }
        cout << "\n";
    }
}

// Helper function to label graph nodes meaningfully (e.g., districts).
string ResourceFlowGraph::getNodeLabel(int node) {
    return "District " + to_string(node); // Placeholder: customize for district names.
}
