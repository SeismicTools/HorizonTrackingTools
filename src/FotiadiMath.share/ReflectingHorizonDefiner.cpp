#include "ReflectingHorizonDefiner.h"
#include <queue>
#include <unordered_set>
#include "CoordinatesAtCube3d.h"

// TODO: n.romanov функция перегружена, по-хорошему, нужно зарефакторить 
std::vector<int32_t>* ReflectingHorizonDefiner::FindReflectingHorizonsForEntireTrace(const CorrelationDataShare& corrData,
	const CoordinatesInCube& traceCoords, 
	const int32_t& idOfPointAtTrace)
{
	auto numOfTraces = corrData.GetInlineCount() * corrData.GetCrosslineCount();
	auto horizonIds = new std::vector<int32_t>(numOfTraces, -1);
	auto idAtOneDimensionalArray = traceCoords.GetIdAtOneDimensionalArray(corrData.GetCrosslineCount());
	// выставили первую точку поверхности отражающего горизонта
	horizonIds->operator[](idAtOneDimensionalArray) = idOfPointAtTrace;

	std::unordered_set<CoordinatesInCube> visited;
	std::queue<CoordinatesAtCube3d> toVisit;

	CoordinatesAtCube3d firstPoint(traceCoords, idOfPointAtTrace);
	toVisit.push(firstPoint);
	visited.insert(traceCoords);

	while (!toVisit.empty()) 
	{
		CoordinatesAtCube3d current3d = toVisit.front();
		CoordinatesInCube current = current3d.coords2d;
		toVisit.pop();

		auto nearbyEdges = corrData.GetEdgesFromPointWithCoordinates(current);
		for (auto edge : *nearbyEdges) 
		{
			const CoordinatesInCube* notCurrentPoint = nullptr;
			int16_t idOfNotCurrentPointAtTrace; // id точки TO в трассе (в сигналах трассы). Тобишь id той точки, на которую отображается текущая
			if(edge->GetPointFrom() == current)
			{
				notCurrentPoint = &edge->GetPointTo();
				idOfNotCurrentPointAtTrace = edge->GetCorrelationIdForFromById(current3d.depth);
			}
			else
			{
				notCurrentPoint = &edge->GetPointFrom();
				idOfNotCurrentPointAtTrace = edge->GetCorrelationIdForToById(current3d.depth);
			}

			if(idOfNotCurrentPointAtTrace < 0)
			{
				// ребро от cur трассы в другую трассу есть,
				// но точка cur трассы никуда не отображается
				continue;
			}

			// если точка не была посещена
			if(visited.find(*notCurrentPoint) == visited.end())
			{
				CoordinatesAtCube3d firstPoint(*notCurrentPoint, idOfNotCurrentPointAtTrace);
				toVisit.push(firstPoint);
				visited.insert(*notCurrentPoint);

				idAtOneDimensionalArray = notCurrentPoint->GetIdAtOneDimensionalArray
					(corrData.GetCrosslineCount());
				horizonIds->operator[](idAtOneDimensionalArray) = idOfNotCurrentPointAtTrace;
			}
		}
		delete nearbyEdges;
	}
	// TODO: n.romanov в конечном итоге мы получим отображение id -> id, а нужно time -> time?
	return horizonIds;
}
