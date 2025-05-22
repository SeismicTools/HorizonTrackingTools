#include "Helper.h"

CorrelationDataShare* Helper::GetCorrelationDataShareObject()
{
	constexpr auto noMappingValue = std::numeric_limits<int16_t>::min();

	std::vector<CoordinatesInCube> allCoordsOfPoints;
	CoordinatesInCube point00(0, 0);
	CoordinatesInCube point01(0, 1);
	CoordinatesInCube point10(1, 0);
	CoordinatesInCube point11(1, 1);
	allCoordsOfPoints.push_back(point00);
	allCoordsOfPoints.push_back(point01);
	allCoordsOfPoints.push_back(point10);
	allCoordsOfPoints.push_back(point11);
	std::vector<S3DEdge> graphEdges;

	std::vector<int16_t> _correlationPointFrom00To01;
	_correlationPointFrom00To01.push_back(noMappingValue); // 0 -> x
	_correlationPointFrom00To01.push_back(0); // 1 -> 0
	_correlationPointFrom00To01.push_back(1); // 2 -> 1
	std::vector<int16_t> _correlationPointFrom01To00;
	_correlationPointFrom01To00.push_back(0); // 0 -> 1
	_correlationPointFrom01To00.push_back(2); // 1 -> 2
	_correlationPointFrom01To00.push_back(noMappingValue); // 2 -> x
	S3DEdge s0(point00, point01, _correlationPointFrom00To01, _correlationPointFrom01To00, 0, 0);

	std::vector<int16_t> _correlationPointFrom00To10;
	_correlationPointFrom00To10.push_back(noMappingValue); // 0 -> x
	_correlationPointFrom00To10.push_back(0); // 1 -> 0
	_correlationPointFrom00To10.push_back(1); // 2 -> 1
	std::vector<int16_t> _correlationPointFrom10To00;
	_correlationPointFrom10To00.push_back(1); // 0 -> 1
	_correlationPointFrom10To00.push_back(2); // 1 -> 2
	_correlationPointFrom10To00.push_back(noMappingValue); // 2 -> x
	S3DEdge s1(point00, point10, _correlationPointFrom00To10, _correlationPointFrom10To00, 0, 0);

	std::vector<int16_t> _correlationPointFrom10To11;
	_correlationPointFrom10To11.push_back(noMappingValue); // 0 -> x
	_correlationPointFrom10To11.push_back(0); // 1 -> 0
	_correlationPointFrom10To11.push_back(1); // 2 -> 1
	std::vector<int16_t> _correlationPointFrom11To10;
	_correlationPointFrom11To10.push_back(1); // 0 -> 1
	_correlationPointFrom11To10.push_back(2); // 1 -> 2
	_correlationPointFrom11To10.push_back(noMappingValue); // 2 -> x
	S3DEdge s2(point10, point11, _correlationPointFrom10To11, _correlationPointFrom11To10, 0, 0);

	graphEdges.push_back(s0);
	graphEdges.push_back(s1);
	graphEdges.push_back(s2);
	CorrelationDataShare* corDataShare = new CorrelationDataShare(allCoordsOfPoints, 
		graphEdges, 
		SizeOfCubeByY, 
		SizeOfCubeByX);
	return corDataShare;
}
