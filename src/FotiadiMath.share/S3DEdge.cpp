#include "S3DEdge.h"

S3DEdge::S3DEdge(CoordinatesInCube pointFrom, 
	CoordinatesInCube pointTo,
	std::vector<int16_t>& correlationPointFromOnTo,
	std::vector<int16_t>& correlationPointToOnFrom,
	int32_t pointFromStartId,
	int32_t pointToStartId
)
	: _pointFrom(pointFrom), 
		_pointTo(pointTo),
		_correlationPointFromOnTo(correlationPointFromOnTo),
		_correlationPointToOnFrom(correlationPointToOnFrom),
		_pointFromStartId(pointFromStartId),
		_pointToStartId(pointToStartId)
{

}

const CoordinatesInCube& S3DEdge::GetPointFrom() const
{
	return _pointFrom;
}

const int32_t S3DEdge::GetIdWherePointFromStarts() const
{
	return _pointFromStartId;
}

const int32_t S3DEdge::GetOneDimensionalIdOfFromPoint(const int32_t& numOfElemAtOneRaw) const
{
	return _pointFrom.GetIdAtOneDimensionalArray(numOfElemAtOneRaw);
}

const int32_t S3DEdge:: GetOneDimensionalIdOfToPoint(const int32_t& numOfElemAtOneRaw) const
{
	return _pointTo.GetIdAtOneDimensionalArray(numOfElemAtOneRaw);
}

const int32_t S3DEdge::GetIdWherePointToStarts() const
{
	return _pointToStartId;
}

const CoordinatesInCube& S3DEdge::GetPointTo() const
{
	return _pointTo;
}

const int16_t S3DEdge::GetCorrelationIdForFromById(int32_t id) const
{
	return _correlationPointFromOnTo[id];
}

const int16_t S3DEdge::GetCorrelationIdForToById(int32_t id) const
{
	return _correlationPointToOnFrom[id];
}

const int32_t S3DEdge::GetNumOfSignalsAtOneTrace() const
{
	return _correlationPointFromOnTo.size();
}

const std::vector<int16_t>& S3DEdge::GetAllCorrelationPointsFromOnTo() const
{
	return _correlationPointFromOnTo;
}

const std::vector<int16_t>& S3DEdge::GetAllCorrelationPointsToOnFrom() const
{
	return _correlationPointToOnFrom;
}
