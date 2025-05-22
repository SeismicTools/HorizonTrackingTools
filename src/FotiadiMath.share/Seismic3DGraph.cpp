#include "Seismic3DGraph.h"

Seismic3DGraph::Seismic3DGraph(const DataToFillGraphPoints& data)
	: _ilCount(data.GetNTraceY()),
	_clCount(data.GetNTraceX()),
	_countOfFixedBorders(data.GetCountOfFixedBorders()),
	_fixedPointsIdsForEachTrace(data.GetFixedPointsIdsForEachTrace())
{
	_pointsOfGraphByCoordinatesMap = getGraphPointsWithData(data);
	fillEmptyBorders(data);
	resizeTraces(data);
	fillNeighboursForAllPoints(data);
}

const std::unordered_map<CoordinatesInCube,
	Seismic3DPointWithNeighboursAndSimilarityValues*>& Seismic3DGraph::GetMapWithPointsOfGraph() const
{
	return *_pointsOfGraphByCoordinatesMap;
}

const std::vector<Seismic3DPointWithNeighboursAndSimilarityValues*>* 
	Seismic3DGraph::GetPointsOfGraph() const
{
	auto result = new std::vector<Seismic3DPointWithNeighboursAndSimilarityValues*>(_pointsOfGraphByCoordinatesMap->size());
	int counter = 0;
	for (auto it = _pointsOfGraphByCoordinatesMap->begin(); it != _pointsOfGraphByCoordinatesMap->end(); ++it) {
		result->operator[](counter) = it->second;
		counter++;
	}

	return result;
}

const Seismic3DPointWithNeighboursAndSimilarityValues*
	Seismic3DGraph::TryToGetPointByCoords(CoordinatesInCube& coords) const
{
	auto it = _pointsOfGraphByCoordinatesMap->find(coords);
	if (it != _pointsOfGraphByCoordinatesMap->end()) {
		const auto& foundSeismPoint = it->second;
		return foundSeismPoint;
	}
	return nullptr;
}

CoordinatesInCube Seismic3DGraph::DefineCoordsAtCubeByOneDimensionalArrayId(const int& id) const
{
	int crosslineCoord = id % GetClCount();
	int inlineCoord = id / GetClCount();

	return CoordinatesInCube(inlineCoord, crosslineCoord);
}

const int32_t& Seismic3DGraph::GetIlCount() const
{
	return _ilCount;
}

const int32_t& Seismic3DGraph::GetClCount() const
{
	return _clCount;
}

std::vector<CoordinatesInCube>* Seismic3DGraph::GetAllExistingTraceCoordinates() const
{
	auto keys = new std::vector<CoordinatesInCube>();
	keys->reserve(_pointsOfGraphByCoordinatesMap->size());

	for (const auto& pair : *_pointsOfGraphByCoordinatesMap) {
		//CoordinatesInCube a(pair.first.inlineId, pair.first.crosslineId, pair.second->GetIdWhereTraceStarts());
		keys->push_back(pair.first);
	}

	return keys;
}

Seismic3DGraph::~Seismic3DGraph()
{
	for (auto& pair : *_pointsOfGraphByCoordinatesMap)
	{
		delete pair.second;
	}
	delete _pointsOfGraphByCoordinatesMap;
}

std::unordered_map<CoordinatesInCube,
	Seismic3DPointWithNeighboursAndSimilarityValues*>* 
		Seismic3DGraph::getGraphPointsWithData(const DataToFillGraphPoints& data) const
{
	auto result = new std::unordered_map<CoordinatesInCube,
		Seismic3DPointWithNeighboursAndSimilarityValues*>();

	for (int32_t y = 0; y < data.GetNTraceY(); ++y) // проход по inline`ам
	{ 
		for (int32_t x = 0; x < data.GetNTraceX(); ++x) // проход по crossline`ам
		{ 
			auto signalsForThisTrace = new std::vector<double>(data.GetNTime());
			for (int32_t t = 0; t < data.GetNTime(); ++t) {
				auto signal_value = data.GetSignals()[
					x * data.GetNTraceY() * data.GetNTime()
					+ y * data.GetNTime() 
					+ t];
				(*signalsForThisTrace)[t] = signal_value;
			}
			CoordinatesInCube coords(y, x);
			auto newSeismPoint
				= new Seismic3DPointWithNeighboursAndSimilarityValues(coords, signalsForThisTrace);
			result->insert(std::make_pair(coords, newSeismPoint));
		}
	}

	return result;
}

void Seismic3DGraph::fillEmptyBorders(const DataToFillGraphPoints& data)
{
	int maxValue = _ilCount * _clCount * _countOfFixedBorders;

	const std::vector<int16_t>* mainTraceBorders = nullptr;
	bool canEnd = false;
	for (int y = 0; y < _ilCount; y++)
	{
		for (int x = 0; x < _clCount; x++)
		{
			CoordinatesInCube pointCoords(y, x);
			Seismic3DPointWithNeighboursAndSimilarityValues& point = *TryToGetPointByCoordsPrivate(pointCoords);
			mainTraceBorders = data.GetIdsOfFixedPointsForTraceWithCoordinatesWithOffset(
				point.getCoordinates().inlineId, 
				point.getCoordinates().crosslineId, 
				point.GetIdWhereTraceStarts()
			);
			for(auto elem : *mainTraceBorders)
			{
				if (elem <= -1) // граница не определена 
				{
					mainTraceBorders = nullptr;
					break;
				}
			}

			if(mainTraceBorders != nullptr)
				break;
		}
	}

	if(mainTraceBorders == nullptr)
		return;

	for (int il = 0; il < _ilCount; il++)
	{
		for (int cl = 0; cl < _clCount; cl++)
		{
			for (int horizonNum = 0; horizonNum < _countOfFixedBorders; horizonNum++)
			{
				int32_t id = il
					+ _ilCount * cl
					+ _ilCount * _clCount * horizonNum;
				if (_fixedPointsIdsForEachTrace[id] <= -1) // не определена
				{
					int32_t neighbourId = (il - 1)
						+ _ilCount * cl
						+ _ilCount * _clCount * horizonNum;
					int32_t neighbourIdNext = (il + 1)
						+ _ilCount * cl
						+ _ilCount * _clCount * horizonNum;
					if (neighbourId < 0 || neighbourId > maxValue || neighbourIdNext > maxValue || neighbourIdNext < 0)
					{
						_fixedPointsIdsForEachTrace[id] = mainTraceBorders->operator[](horizonNum);
					}
					else
					{
						int32_t avg = (_fixedPointsIdsForEachTrace[neighbourIdNext] - _fixedPointsIdsForEachTrace[neighbourId]) / 2 + _fixedPointsIdsForEachTrace[neighbourId];
						_fixedPointsIdsForEachTrace[id] = _fixedPointsIdsForEachTrace[neighbourId];
					}
				}
				else if(_fixedPointsIdsForEachTrace[id] >= data.GetNTime()) // выходит за пределы выборки
				{
					_fixedPointsIdsForEachTrace[id] = data.GetNTime() - 1;
				}
			}
		}
	}
}

void Seismic3DGraph::fillNeighboursForAllPoints(const DataToFillGraphPoints& data)
{
	auto callback = data.GetCallbackProgressReporter();
	auto totalNeedToProcess = _ilCount * _clCount;
	for(int y = 0; y < _ilCount; y++)
	{
		for(int x = 0; x < _clCount; x++)
		{
			CoordinatesInCube pointCoords(y, x);
			Seismic3DPointWithNeighboursAndSimilarityValues& point = *TryToGetPointByCoordsPrivate(pointCoords);
			auto neighboursOfPoint = getNeighboursOfPoint(point);

			point.addNeighboursAndCountSimilarityForThem(*neighboursOfPoint, 
				data);

			delete neighboursOfPoint;

			double progressValue = (x + y * _clCount) / (double)totalNeedToProcess;
			auto wasCancelled = callback(progressValue, data.GetCallbackData());

			if(wasCancelled)
			{
				return;
			}
		}
	}
}

void Seismic3DGraph::resizeTraces(const DataToFillGraphPoints& data)
{
	auto callback = data.GetCallbackProgressReporter();
	auto totalNeedToProcess = _ilCount * _clCount;
	for (int y = 0; y < _ilCount; y++)
	{
		for (int x = 0; x < _clCount; x++)
		{
			CoordinatesInCube pointCoords(y, x);
			Seismic3DPointWithNeighboursAndSimilarityValues& point = *TryToGetPointByCoordsPrivate(pointCoords);
			point.resizeTrace(data);
		}
	}
}

// TODO: n.romanov сейчас соседи по горизонтали не рассматриваются
std::vector<Seismic3DPointWithNeighboursAndSimilarityValues*>*
	Seismic3DGraph::getNeighboursOfPoint(const Seismic3DPointWithNeighboursAndSimilarityValues& point) const
{
	auto result = new std::vector<Seismic3DPointWithNeighboursAndSimilarityValues*>;

	auto coords = point.getCoordinates();

	// мы не добавляем соседей сверху и слева, т.к. ожидается, что их нам уже 
	// добавили, вызвая данный метод на точках, которые как раз таки сверху и слева
	auto coordsDown = CoordinatesInCube(coords.inlineId + 1, coords.crosslineId);
	auto coordsRight = CoordinatesInCube(coords.inlineId, coords.crosslineId + 1);

	auto pointDown = TryToGetPointByCoordsPrivate(coordsDown);
	auto pointRight = TryToGetPointByCoordsPrivate(coordsRight);

	pointDown != nullptr ? result->push_back(pointDown) : void();
	pointRight != nullptr ? result->push_back(pointRight) : void();

	return result;
}

Seismic3DPointWithNeighboursAndSimilarityValues* Seismic3DGraph::TryToGetPointByCoordsPrivate(const CoordinatesInCube& coords) const
{
	auto it = _pointsOfGraphByCoordinatesMap->find(coords);
	if (it != _pointsOfGraphByCoordinatesMap->end()) {
		const auto& foundSeismPoint = it->second;
		return foundSeismPoint;
	}
	return nullptr;
}
