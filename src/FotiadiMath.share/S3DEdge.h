#pragma once

#include "CoordinatesInCube.h"

/// <summary>
/// Ребра ненаправленные
/// </summary>
class S3DEdge
{
	public:
		S3DEdge(CoordinatesInCube pointFrom, 
			CoordinatesInCube pointTo,
			std::vector<int16_t>& correlationPointFromOnTo,
			std::vector<int16_t>& correlationPointToOnFrom,
			int32_t pointFromStartId,
			int32_t pointToStartId);
		const CoordinatesInCube& GetPointFrom() const;
		const CoordinatesInCube& GetPointTo() const;
		const int16_t GetCorrelationIdForFromById(int32_t id) const;
		const int16_t GetCorrelationIdForToById(int32_t id) const;
		const int32_t GetNumOfSignalsAtOneTrace() const;
		const std::vector<int16_t>& GetAllCorrelationPointsFromOnTo() const;
		const std::vector<int16_t>& GetAllCorrelationPointsToOnFrom() const;
		const int32_t GetIdWherePointFromStarts() const;
		const int32_t GetIdWherePointToStarts() const;
		const int32_t GetOneDimensionalIdOfFromPoint(const int32_t& numOfElemAtOneRaw) const;
		const int32_t GetOneDimensionalIdOfToPoint(const int32_t& numOfElemAtOneRaw) const;

	private:
		const CoordinatesInCube _pointFrom;
		const int32_t _pointFromStartId;
		const CoordinatesInCube _pointTo;
		const int32_t _pointToStartId;
		// TODO: n.romanov хорошо бы векторы не копировать, а таскать указатель на них
		std::vector<int16_t> _correlationPointFromOnTo;
		std::vector<int16_t> _correlationPointToOnFrom;
};
