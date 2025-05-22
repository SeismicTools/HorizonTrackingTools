#pragma once

#include "PrimSpanningTree.h"
#include "Seismic3DGraph.h"
#include "S3DEdge.h"

class S3GraphPrimSpanningTreeAdapter
{
	public:
		static PrimSpanningTree* GetPrimSpanningTree(Seismic3DGraph& s3Graph);

		static std::vector<S3DEdge>* GetS3DEdgesAtSpanningTree(Seismic3DGraph& s3Graph);
};
