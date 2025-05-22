#pragma once

#include <cstdint>
#include <vector>
#include "CoordinatesInCube.h"

// говорим, что класс определен в другом хедере
// (forward-declaration)
class Seismic3DPointWithNeighboursAndSimilarityValues;

class Seismic3DPointNeighbour {
	public:
		Seismic3DPointNeighbour(const Seismic3DPointWithNeighboursAndSimilarityValues& point, 
			const double& similarity,
			const std::vector<int16_t>& mappingTraceOnThisNeighbour);

		const Seismic3DPointWithNeighboursAndSimilarityValues& GetPoint() const;

		const double& GetSimilarityValue() const;

		const int32_t GetIdAtOneDimensionalArray(const int32_t& numOfElemAtOneRaw) const;

		const CoordinatesInCube& GetCoordinates() const;

		const std::vector<int16_t>& GetMappingSomeTraceOnThisNeighbor() const;

	private:
		const Seismic3DPointWithNeighboursAndSimilarityValues& _point;
		const double _similarity;
		/// <summary>
		/// Здесь хранится отображение из той точки, которая хранит этого соседа.
		/// В точку _point
		/// </summary>
		const std::vector<int16_t> _mappingTraceOnThisNeighbour;
};
