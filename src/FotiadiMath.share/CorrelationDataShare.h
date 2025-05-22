#pragma once

#include <unordered_set>
#include "S3DEdge.h"
#include <fstream>
#include <iostream>
#include <filesystem>  
#include <unordered_map>
#include <ArgumentIllegalException.h>
#include <queue>

class CorrelationDataShare
{
	public:
		CorrelationDataShare(const std::vector<CoordinatesInCube>& allCoordsOfPoints,
			const std::vector<S3DEdge>& graphEdges,
			const int32_t& inlineCount,
			const int32_t& crosslineCount);

		bool isContainPoint(const CoordinatesInCube& coords) const;

		int32_t GetInlineCount() const;

		int32_t GetCrosslineCount() const;

		const std::vector<const S3DEdge*>* GetEdgesFromPointWithCoordinates(
			const CoordinatesInCube& pointCoords) const;

		void SaveGraphToCsv() const;

		int32_t GetDepthOfTree(int32_t inlineRef, int32_t crosslineRef) const;
		void SaveDepthOfTreeToFile(int32_t inlineRef, int32_t crosslineRef) const;

	private:
		std::unordered_set<CoordinatesInCube> _knownTraceCoordinates;
		std::vector<S3DEdge> _graphEdges; // рёбра в графе неориентированные
		int32_t _inlineCount;
		int32_t _crosslineCount;
};
