#include "Seismic3DPointNeighbour.h"
#include "Seismic3DPointWithNeighboursAndSimilarityValues.h"

Seismic3DPointNeighbour::Seismic3DPointNeighbour(const Seismic3DPointWithNeighboursAndSimilarityValues& point, 
	const double& similarity,
	const std::vector<int16_t>& mappingTraceOnThisNeighbour)
	: _point(point), _similarity(similarity),
		_mappingTraceOnThisNeighbour(mappingTraceOnThisNeighbour)
{

}

const Seismic3DPointWithNeighboursAndSimilarityValues& Seismic3DPointNeighbour::GetPoint() const
{
	return _point;
}

const double& Seismic3DPointNeighbour::GetSimilarityValue() const
{
	return _similarity;
}

const int32_t Seismic3DPointNeighbour::GetIdAtOneDimensionalArray(const int32_t& numOfElemAtOneRaw) const
{
	return _point.GetIdAtOneDimensionalArray(numOfElemAtOneRaw);
}

const CoordinatesInCube& Seismic3DPointNeighbour::GetCoordinates() const
{
	return _point.GetCoordinatesByRef();
}

const std::vector<int16_t>& Seismic3DPointNeighbour::GetMappingSomeTraceOnThisNeighbor() const
{
	return _mappingTraceOnThisNeighbour;
}
