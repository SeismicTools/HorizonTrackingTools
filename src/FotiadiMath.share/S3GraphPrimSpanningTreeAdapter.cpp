#include "S3GraphPrimSpanningTreeAdapter.h"

PrimSpanningTree* S3GraphPrimSpanningTreeAdapter::GetPrimSpanningTree(Seismic3DGraph& s3Graph)
{
	const std::unordered_map<CoordinatesInCube,
		Seismic3DPointWithNeighboursAndSimilarityValues*>& mapOfS3Graph = s3Graph.GetMapWithPointsOfGraph();

	int vertexCount = mapOfS3Graph.size();
	std::vector<std::vector<AdjacencyListUnit*>> graph(vertexCount);

	auto ilCount = s3Graph.GetIlCount();
	auto clCount = s3Graph.GetClCount();
	for(int y = 0; y < ilCount; y++)
	{
		for(int x = 0; x < clCount; x++)
		{
			CoordinatesInCube pointCoords(y, x);
			const Seismic3DPointWithNeighboursAndSimilarityValues* point = s3Graph.TryToGetPointByCoords(pointCoords);
			auto fromId = point->GetIdAtOneDimensionalArray(clCount);
			if(point != nullptr)
			{
				auto neighbors = point->GetNeighborsAndSimilarities();
				for(auto& neighbor : neighbors)
				{
					auto pointId = neighbor->GetIdAtOneDimensionalArray(clCount);
					auto weight = neighbor->GetSimilarityValue();
					auto adjUnitTo = new AdjacencyListUnit(weight, pointId);
					graph[fromId].push_back(adjUnitTo);
				}
			}
			else
			{
				throw std::runtime_error("TryToGetPointByCoords shouldn't return nullptr here.");
			}
		}
	}

	auto result = PrimSpanningTree::Build(graph);

	for(int i = 0; i < ilCount; i++)
	{
		for(AdjacencyListUnit* unit : graph[i])
		{
			delete unit;
		}
	}

	return result;
}

std::vector<S3DEdge>* S3GraphPrimSpanningTreeAdapter::GetS3DEdgesAtSpanningTree(Seismic3DGraph& s3Graph)
{
	auto primTree = GetPrimSpanningTree(s3Graph);
	auto edges = primTree->GetPrimSpanningTree();

	auto result = new std::vector<S3DEdge>();
	for(Edge* edge : edges)
	{
		auto coordsFrom = s3Graph.DefineCoordsAtCubeByOneDimensionalArrayId(edge->_vertexIdFrom);
		auto coordsTo = s3Graph.DefineCoordsAtCubeByOneDimensionalArrayId(edge->_vertexIdTo);

		auto pointFrom = s3Graph.TryToGetPointByCoords(coordsFrom);
		auto foundNeighborPoint = pointFrom->TryToFindNeighborPointByCoords(coordsTo);
		auto correlationToNeighbor = foundNeighborPoint->GetMappingSomeTraceOnThisNeighbor();

		auto pointTo = s3Graph.TryToGetPointByCoords(coordsTo);
		foundNeighborPoint = pointTo->TryToFindNeighborPointByCoords(coordsFrom);
		auto correlationBack = foundNeighborPoint->GetMappingSomeTraceOnThisNeighbor();

		S3DEdge newEdge(coordsFrom, coordsTo, correlationToNeighbor, correlationBack
			, pointFrom->GetIdWhereTraceStarts()
			, pointTo->GetIdWhereTraceStarts());
		result->push_back(newEdge);
	}

	delete primTree;
	return result;
}
