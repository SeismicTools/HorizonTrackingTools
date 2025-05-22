#include "ReflectingHorizonDefinerForEntireTrace.h"
#include <queue>
#include <HorizonWrapper.h>
#include <stdexcept>
#include <thread>

int32_t* ReflectingHorizonDefinerForEntireTrace::FindReflectingHorizonsForEntireTrace(
	const int32_t& traceCoordByX, 
	const int32_t& traceCoordByY, 
	const CorrelationDataShare& correlationData)
{
	const int32_t numOfTracesAtOneRaw = correlationData.GetCrosslineCount();
	const int32_t inlineCount = correlationData.GetInlineCount();
	const int32_t numOfTracesAtCube = correlationData.GetCrosslineCount() * correlationData.GetInlineCount();

	const CoordinatesInCube initialTraceCoord(traceCoordByY, traceCoordByX);
	auto edges = correlationData.GetEdgesFromPointWithCoordinates(initialTraceCoord);

	if(edges->size() == 0)
	{
		delete edges;
		return nullptr;
	}

	const int32_t numOfSignalsAtOneTrace = edges->operator[](0)->GetNumOfSignalsAtOneTrace();

	// numOfSignalsAtOneTrace - для каждого сигнала, своя поверхность - numOfTracesAtCube
	HorizonWrapper horizons(numOfSignalsAtOneTrace, 
		numOfTracesAtCube,
		numOfTracesAtOneRaw,
		inlineCount);

	// в поверхность каждого сигнала ставим первую точку с номером границы
	for(int i = 0, horizonNum = 1; i < numOfSignalsAtOneTrace; i++)
	{
		horizons.SetHorizon(traceCoordByX, traceCoordByY, i, horizonNum);

		if(horizonNum > 0)
		{
			horizonNum = -(horizonNum + 1);
		}
		else
		{
			horizonNum = -(horizonNum - 1);
		}
	}

	// теперь надо посчитать для всех остальных трасс, кроме начальной, полную поверхность
	// DFS
	std::unordered_set<CoordinatesInCube> visited;
	std::queue<CoordinatesInCube> toVisit;

	CoordinatesInCube firstPoint(traceCoordByY, traceCoordByX);
	toVisit.push(firstPoint);
	visited.insert(firstPoint);

	while (!toVisit.empty())
	{
		CoordinatesInCube current = toVisit.front();
		toVisit.pop();

		auto nearbyEdges = correlationData.GetEdgesFromPointWithCoordinates(current);
		for (auto edge : *nearbyEdges)
		{
			const CoordinatesInCube* notCurrentPoint = nullptr;
			const std::vector<int16_t>* mapping = nullptr;
			if (edge->GetPointFrom() == current)
			{
				notCurrentPoint = &edge->GetPointTo();
				mapping = &edge->GetAllCorrelationPointsFromOnTo();
			}
			else
			{
				notCurrentPoint = &edge->GetPointFrom();
				mapping = &edge->GetAllCorrelationPointsToOnFrom();
			}

			// если точка не была посещена
			if (visited.find(*notCurrentPoint) == visited.end())
			{
				// нашли точку, куда ведет ребро из текущей точки
				// также нашли отображение из точки в точку
				// под точкой подразумевается трасса

				// обход всех точек трассы
				for (int i = 0; i < numOfSignalsAtOneTrace; i++)
				{
					// если в текущей границе стоит min_value - skip (т.к. нет границы для значения)
					auto horizon = horizons.GetHorizon(current.inlineId, current.crosslineId, i);
					if (horizon == HorizonWrapper::NothingValue)
					{
						continue;
					}
					// если никуда не ведет (нет отображения) - skip
					if (mapping->operator[](i) < 0)
					{
						continue;
					}

					auto depth = mapping->operator[](i);

					// отображение есть, значит ставим в horizons границу
					horizons.SetHorizon(notCurrentPoint->crosslineId, notCurrentPoint->inlineId, depth, horizon);
				}

				// добавление точек для будущего посещения DFS
				toVisit.push(*notCurrentPoint);
				visited.insert(*notCurrentPoint);
			}
		}
		delete nearbyEdges;
	}

	delete edges;
	return horizons.GetHorizonsData();
}

int32_t* ReflectingHorizonDefinerForEntireTrace::FindReflectingHorizonsForEntireTrace(
	const int32_t& traceCoordByInline, // inline
	const int32_t& traceCoordByCrossline, // crossline
	const CorrelationDataShare& correlationData, 
	const int32_t countOfFixedBorders, 
	int16_t surface_idx[], 
	const int32_t spacesBetweenTwoHorizons, 
	const int32_t sizeOfOneHorizon,
	const void* callbackData, // тут будет указатель на объект класса PluginProgressReporterInterface
	setPorgressValueFunc callback,
	const int32_t horizonsBreadth			// ширина границ
)
{
	const int32_t crosslineCount = correlationData.GetCrosslineCount();
	const int32_t inlineCount = correlationData.GetInlineCount();
	const int32_t numOfTracesAtCube = correlationData.GetCrosslineCount() * correlationData.GetInlineCount();

	const CoordinatesInCube initialTraceCoord(traceCoordByInline, traceCoordByCrossline);
	auto edges = correlationData.GetEdgesFromPointWithCoordinates(initialTraceCoord);

	if (edges->size() == 0)
	{
		delete edges;
		return nullptr;
	}

	const int32_t numOfSignalsAtOneTrace = edges->operator[](0)->GetNumOfSignalsAtOneTrace();

	// numOfSignalsAtOneTrace - для каждого сигнала, своя поверхность - numOfTracesAtCube
	HorizonWrapper horizons(numOfSignalsAtOneTrace,
		numOfTracesAtCube,
		crosslineCount,
		inlineCount);

	// в поверхность каждого сигнала ставим первую точку с номером границы
	//int32_t depth = 0;
	//int32_t maxDepth = numOfSignalsAtOneTrace;
	//if(countOfFixedBorders > 1) // чтобы заполнитель данные только между границами
	//{
	//	int surfaceNum = 0;
	//	int tempDepth = surface_idx[0];
	//	if(tempDepth < maxDepth) // чтобы depth не ушел за границу от 0 до maxDepth
	//	{
	//		depth = tempDepth;
	//	}

	//	surfaceNum = countOfFixedBorders - 1; // самая нижняя граница
	//	tempDepth = surface_idx[surfaceNum];
	//	if(tempDepth < numOfSignalsAtOneTrace)
	//	{
	//		maxDepth = tempDepth;
	//	}
	//	else
	//	{
	//		depth = 0;
	//	}
	//}

	auto minId = surface_idx[0];
	auto maxId = surface_idx[countOfFixedBorders - 1];
	if(minId < 0)
		minId = 0;
	if(maxId >= numOfSignalsAtOneTrace)
		maxId = numOfSignalsAtOneTrace - 1;
	
	// заполнение опорной трассы номерами границ
	for(int32_t up = minId, down = maxId-1, counter = 0, horizonNum = 10; up < down;)
	{
		horizons.SetHorizon(traceCoordByInline, traceCoordByCrossline, up, horizonNum);
		up++;
		counter++;
		if(counter == horizonsBreadth)
		{
			counter = 0;
			if(horizonNum > 0)
			{
				horizonNum += 10;
			}
			else
			{
				horizonNum -= 10;
			}
			horizonNum *= -1;
			up += spacesBetweenTwoHorizons;
		}
	}

	// теперь надо посчитать для всех остальных трасс, кроме начальной, полную поверхность
	// DFS
	std::unordered_set<CoordinatesInCube> visited;
	std::queue<CoordinatesInCube> toVisit;

	CoordinatesInCube firstPoint(traceCoordByInline, traceCoordByCrossline);
	toVisit.push(firstPoint);
	visited.insert(firstPoint);

	int32_t finishedToCalculateNum = 0;
	while (!toVisit.empty())
	{
		CoordinatesInCube current = toVisit.front();
		toVisit.pop();

		auto nearbyEdges = correlationData.GetEdgesFromPointWithCoordinates(current);
		for (auto edge : *nearbyEdges)
		{
			const CoordinatesInCube* notCurrentPoint = nullptr;
			int32_t notCurrentPointIdWhereStarts = 0;
			const std::vector<int16_t>* mapping = nullptr;
			if (edge->GetPointFrom() == current)
			{
				notCurrentPoint = &edge->GetPointTo();
				mapping = &edge->GetAllCorrelationPointsFromOnTo();
				//mapping = &edge->GetAllCorrelationPointsToOnFrom();
				notCurrentPointIdWhereStarts = edge->GetIdWherePointToStarts();
				//notCurrentPointIdWhereStarts = edge->GetIdWherePointFromStarts();
			}
			else if(edge->GetPointTo() == current)
			{
				notCurrentPoint = &edge->GetPointFrom();
				mapping = &edge->GetAllCorrelationPointsToOnFrom();
				//mapping = &edge->GetAllCorrelationPointsFromOnTo();
				notCurrentPointIdWhereStarts = edge->GetIdWherePointFromStarts();
				//notCurrentPointIdWhereStarts = edge->GetIdWherePointToStarts();
			}
			else
			{
				int a = 5+5;
			}

			// если точка не была посещена
			if (visited.find(*notCurrentPoint) == visited.end())
			{
				// нашли точку, куда ведет ребро из текущей точки
				// также нашли отображение из точки в точку
				// под точкой подразумевается трасса

				// обход всех точек трассы
				for (int i = 0; i < numOfSignalsAtOneTrace; i++)
				{
					// если в текущей границе стоит min_value - skip (т.к. нет границы для значения)
					auto horizon = horizons.GetHorizon(current.inlineId, current.crosslineId, i);
					if (horizon == HorizonWrapper::NothingValue)
					{
						continue;
					}
					// если никуда не ведет (нет отображения) - skip
					if (mapping->operator[](i) < 0)
					{
						continue;
					}

					auto depth = mapping->operator[](i);

					// отображение есть, значит ставим в horizons границу
					horizons.SetHorizon(notCurrentPoint->inlineId, 
						notCurrentPoint->crosslineId, 
						depth + notCurrentPointIdWhereStarts, 
						horizon
					);
				}

				// добавление точек для будущего посещения DFS
				toVisit.push(*notCurrentPoint);
				visited.insert(*notCurrentPoint);
			}
		}
		delete nearbyEdges;
		finishedToCalculateNum++;

		auto progressValue = (double)finishedToCalculateNum / (double)numOfTracesAtCube;
		auto wasCancelled = callback(progressValue, callbackData);

		//std::this_thread::sleep_for(std::chrono::milliseconds(500));

		if (wasCancelled)
		{
			delete edges;
			std::string bla = "FindReflectingHorizonsForEntireTrace was stopped by UI.";
			throw std::runtime_error(bla);
		}
	}

	// заполнение опорной трассы номерами границ
	for (int32_t up = minId, down = maxId - 1, counter = 0, horizonNum = 10; up < down;)
	{
		horizons.SetHorizon(traceCoordByInline, traceCoordByCrossline, up, horizonNum*100);
		up++;
		counter++;
		if (counter == horizonsBreadth)
		{
			counter = 0;
			if (horizonNum > 0)
			{
				horizonNum += 10;
			}
			else
			{
				horizonNum -= 10;
			}
			horizonNum *= -1;
			up += spacesBetweenTwoHorizons;
		}
	}

	delete edges;
	return horizons.GetHorizonsData();
}
