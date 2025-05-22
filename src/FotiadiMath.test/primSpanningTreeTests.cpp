#include <gtest/gtest.h>
#include "PrimSpanningTree.cpp"

namespace spanning_tree_prim_tests
{
    static bool contains(const std::vector<Edge*>& vec, const Edge& value) {
        return std::find_if(vec.begin(), vec.end(), [value](Edge* edge)
        {
            return value == *edge;
        }) != vec.end();
    }

	static TEST(prim, Ex1) {
        // given graph
		int vertexCount = 4;
		std::vector<std::vector<AdjacencyListUnit*>> graph(vertexCount);

        AdjacencyListUnit v01(2,1);
        graph[0].push_back(&v01);
        AdjacencyListUnit v03(5, 3);
        graph[0].push_back(&v03);
        AdjacencyListUnit v10(2, 0);
        graph[1].push_back(&v10);
        AdjacencyListUnit v12(1, 2);
        graph[1].push_back(&v12);
        AdjacencyListUnit v13(3, 3);
        graph[1].push_back(&v13);
        AdjacencyListUnit v21(1, 1);
        graph[2].push_back(&v21);
        AdjacencyListUnit v23(1, 3);
        graph[2].push_back(&v23);
        AdjacencyListUnit v30(5, 0);
        graph[3].push_back(&v30);
        AdjacencyListUnit v31(3, 1);
        graph[3].push_back(&v31);
        AdjacencyListUnit v32(1, 2);
        graph[3].push_back(&v32);


        // when building spanning tree
        auto minimum_spanning_tree = PrimSpanningTree::Build(graph);

        const std::vector<Edge*>& result = minimum_spanning_tree->GetPrimSpanningTree();

        // then should get spanning tree
        Edge firstCheck(0, 1, 2);
        auto firstEdgeWeight = contains(result, firstCheck);
        EXPECT_EQ(firstEdgeWeight, true);

        Edge secondCheck(1, 2, 1);
        auto secondEdgeWeight = contains(result, secondCheck);
        EXPECT_EQ(secondEdgeWeight, true);

        Edge thirdCheck(2, 3, 1);
        auto thirdEdgeWeight = contains(result, thirdCheck);
        EXPECT_EQ(thirdEdgeWeight, true);

        delete minimum_spanning_tree;
	}
}
