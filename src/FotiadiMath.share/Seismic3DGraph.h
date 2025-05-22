#pragma once

#include <unordered_map>
#include <CoordinatesInCube.h>
#include <Seismic3DPointWithNeighboursAndSimilarityValues.h>
#include <DataToFillGraphPoints.h>
#include <Point3DSimilarityHelper.h>
#include <CoordinatesInCubeWithIdWhereTraceStatrs.h>

class Seismic3DGraph {
	public:
		Seismic3DGraph(const DataToFillGraphPoints& data);

		const std::unordered_map<CoordinatesInCube,
			Seismic3DPointWithNeighboursAndSimilarityValues*>& GetMapWithPointsOfGraph() const;

		const std::vector<Seismic3DPointWithNeighboursAndSimilarityValues*>* GetPointsOfGraph() const;

		const int32_t& GetIlCount() const;

		const int32_t& GetClCount() const;

		std::vector<CoordinatesInCube>* GetAllExistingTraceCoordinates() const;

		const Seismic3DPointWithNeighboursAndSimilarityValues* TryToGetPointByCoords(CoordinatesInCube& coords) const;

		CoordinatesInCube DefineCoordsAtCubeByOneDimensionalArrayId(const int& id) const;

		~Seismic3DGraph();

private:
	std::unordered_map<CoordinatesInCube,
		Seismic3DPointWithNeighboursAndSimilarityValues*>* _pointsOfGraphByCoordinatesMap;

	const int32_t _ilCount;
	const int32_t _clCount;
	const int32_t _countOfFixedBorders;
	int16_t* _fixedPointsIdsForEachTrace;

	std::unordered_map<CoordinatesInCube,
		Seismic3DPointWithNeighboursAndSimilarityValues*>* getGraphPointsWithData(const DataToFillGraphPoints& data) const;

	// заполнение пропущенных значений границ
	void fillEmptyBorders(const DataToFillGraphPoints& data);

	void fillNeighboursForAllPoints(const DataToFillGraphPoints& data);

	void resizeTraces(const DataToFillGraphPoints& data);

	std::vector<Seismic3DPointWithNeighboursAndSimilarityValues*>*
		getNeighboursOfPoint(const Seismic3DPointWithNeighboursAndSimilarityValues& point) const;

	Seismic3DPointWithNeighboursAndSimilarityValues* TryToGetPointByCoordsPrivate(const CoordinatesInCube& coords) const;
};
