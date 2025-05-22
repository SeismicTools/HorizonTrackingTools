#pragma once

#include <vector>
#include "AdjacencyListUnit.h"
#include "Edge.h"

/// <summary>
/// Реализация алгоритма Прима (построение остовного дерева).
/// Используется список смежности.
/// </summary>
class PrimSpanningTree
{
	public:
		static PrimSpanningTree* Build(std::vector<std::vector<AdjacencyListUnit*>>& graph);

		const std::vector<Edge*>& GetPrimSpanningTree() const;

		~PrimSpanningTree();

	private:
		int32_t _initialVertexId;
		std::vector<Edge*>* _primSpanningTree;

		PrimSpanningTree();
};
