#include "PrimSpanningTree.h"
#include <queue>

PrimSpanningTree* PrimSpanningTree::Build(std::vector<std::vector<AdjacencyListUnit*>>& graph)
{
	auto numOfVertexAtGraph = graph.size();
	std::vector<bool> visited(numOfVertexAtGraph, false);

	// сортировка по минимальным весам производится за счет очереди с приоритетами
	std::priority_queue<Edge,
		std::vector<Edge>,
			std::greater<Edge>> vertexToCheckSorted;

	auto result = new PrimSpanningTree();
	result->_primSpanningTree = new std::vector<Edge*>();
	result->_initialVertexId = 0;

	int32_t initialVertexId = result->_initialVertexId;
	Edge initialEdge(initialVertexId, initialVertexId, 0);
	vertexToCheckSorted.push(initialEdge);

	while (!vertexToCheckSorted.empty()) 
	{
		auto weight = vertexToCheckSorted.top()._weight;
		auto nodeTo = vertexToCheckSorted.top()._vertexIdTo; // в текущий момент времени проверяем вершину под этим id
		auto nodeFrom = vertexToCheckSorted.top()._vertexIdFrom;
		vertexToCheckSorted.pop();

		if (visited[nodeTo]) {
			continue;
		}

		visited[nodeTo] = true;
		if(nodeTo != initialVertexId)
		{
			AdjacencyListUnit vertex(weight, nodeTo);
			auto edge = new Edge(nodeFrom, nodeTo, weight);
			result->_primSpanningTree->push_back(edge);
		}

		for (AdjacencyListUnit* neighbor : graph[nodeTo]) {
			auto next_node = neighbor->vertexId;
			auto weight = neighbor->weight;
			if (!visited[next_node]) {
				Edge toCheck(nodeTo, next_node, weight);
				vertexToCheckSorted.push(toCheck);
			}
		}
	}

	return result;
}

const std::vector<Edge*>& PrimSpanningTree::GetPrimSpanningTree() const
{
	return *_primSpanningTree;
}

PrimSpanningTree::~PrimSpanningTree()
{
	for(auto edge : *_primSpanningTree)
	{
		delete edge;
	}

	delete _primSpanningTree;
}

PrimSpanningTree::PrimSpanningTree() : _initialVertexId(0), _primSpanningTree(nullptr)
{
	
};
