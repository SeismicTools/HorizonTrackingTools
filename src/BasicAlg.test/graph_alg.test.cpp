#include <gtest/gtest.h>
#include "graph_alg.h"

TEST(graph_alg, debug) {
    /* Let us create above shown weighted
     and undirected graph */
    int V = 9, E = 14;
    Graph<EdgeCost<double>> g(V);

    // making above shown graph 
    g.addEdge(EdgeCost < double>(0, 1, 4));
    g.addEdge(EdgeCost < double>(0, 7, 8));
    g.addEdge(EdgeCost < double>(1, 2, 8));
    g.addEdge(EdgeCost < double>(1, 7, 11));
    g.addEdge(EdgeCost < double>(2, 3, 7));
    g.addEdge(EdgeCost < double>(2, 8, 2));
    g.addEdge(EdgeCost < double>(2, 5, 4));
    g.addEdge(EdgeCost < double>(3, 4, 9));
    g.addEdge(EdgeCost < double>(3, 5, 14));
    g.addEdge(EdgeCost < double>(4, 5, 10));
    g.addEdge(EdgeCost < double>(5, 6, 2));
    g.addEdge(EdgeCost < double>(6, 7, 1));
    g.addEdge(EdgeCost < double>(6, 8, 6));
    g.addEdge(EdgeCost < double>(7, 8, 7));

    std::cout << "Edges of MST are \n";
 
    Graph<EdgeCost<double>>::kruskalMST(g);

    Graph<Edge> gn(g);
//	EXPECT_EQ(idx, 2);
}