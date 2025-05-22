#include "MainAndOtherPointsData.h"
#include "Seismic3DPointWithNeighboursAndSimilarityValues.h"
#include <ArgumentIllegalException.h>
#include <HorizonWrapper.h>

MainAndOtherPointsData::MainAndOtherPointsData(
	const int32_t inlineCount, 
	const int32_t crosslineCount, 
	const int32_t numOfSignalsAtOneTrace, 
	const double* signals, 
	const int32_t max_shift_point_idx, 
	const int32_t countOfFixedBorders, 
	int16_t* surface_idx, 
	const int16_t* fix_point_idx_r, 
	const void* callbackData, 
	setPorgressValueFunc callback, 
	int32_t r_idx, 
	const int32_t mainTraceCoordByX, 
	const int32_t mainTraceCoordByY,
	const int32_t horizonsBreadth,
	const int32_t spaceBetweenHorizons
) : _inlineCount(inlineCount),
	_crosslineCount(crosslineCount),
	_numOfSignalsAtOneTrace(numOfSignalsAtOneTrace),
	_signals(signals),
	_max_shift_point_idx(max_shift_point_idx),
	_countOfFixedBorders(countOfFixedBorders),
	_surface_idx(surface_idx),
	_fix_point_idx_r(fix_point_idx_r),
	_callbackData(callbackData),
	_callback(callback),
	_r_idx(r_idx),
	_mainTraceCoordByX(mainTraceCoordByX),
	_mainTraceCoordByY(mainTraceCoordByY),
	_horizonsBreadth(horizonsBreadth),
	_spaceBetweenHorizons(spaceBetweenHorizons)
{
	if(_countOfFixedBorders < 2)
	{
		throw new FotiadiMath_Exceptions::ArgumentIllegalException();
	}
}

void MainAndOtherPointsData::CorrelateMainTraceWithAll()
{
	// формируем все точки с сигналами
	auto allPoints = new std::vector<Seismic3DPointWithNeighboursAndSimilarityValues*>();
	Seismic3DPointWithNeighboursAndSimilarityValues* mainPoint = nullptr; // опорная точка

	// границы для опорной трассы
	auto idsOfFixedPointsAtThisTrace = GetIdsOfFixedPointsForTraceWithCoordinates(_mainTraceCoordByX, _mainTraceCoordByY, 0);
	FillEmptyTraces(idsOfFixedPointsAtThisTrace);
	
	for (int32_t inlineNum = 0; inlineNum < _inlineCount; ++inlineNum) // проход по inline`ам
	{
		for (int32_t crosslineNum = 0; crosslineNum < _crosslineCount; ++crosslineNum) // проход по crossline`ам
		{
			// индексы фиксированных границ в этой трассе глобальные
			auto idsOfFixedPointsAtThisTrace = GetIdsOfFixedPointsForTraceWithCoordinates(inlineNum, crosslineNum, 0);
			auto minId = *std::min_element(idsOfFixedPointsAtThisTrace->begin(), idsOfFixedPointsAtThisTrace->end());
			auto maxId = *std::max_element(idsOfFixedPointsAtThisTrace->begin(), idsOfFixedPointsAtThisTrace->end());

			bool canCorrelateThisTrace = true;
			if(minId < 0 || maxId < 0)
			{
				minId = 0;
				maxId = _numOfSignalsAtOneTrace - 1;
				//canCorrelateThisTrace = false;
			}

			if(maxId >= _numOfSignalsAtOneTrace)
				maxId = _numOfSignalsAtOneTrace - 1;

			int totalLenght;
			if(_numOfSignalsAtOneTrace < maxId)
			{
				totalLenght = _numOfSignalsAtOneTrace - minId + 1;
			}
			else // >=
			{
				totalLenght = maxId - minId + 1;
			}
			 
			// использовать это, когда трассы можно будет делать разного размера
			auto signalsForThisTrace = new std::vector<double>(totalLenght);
			int32_t idToWrite = 0;
			for (int32_t t = minId; t <= maxId && t < _numOfSignalsAtOneTrace; ++t, idToWrite++) {
				auto signal_value = _signals[
					crosslineNum * _inlineCount * _numOfSignalsAtOneTrace
						+ inlineNum * _numOfSignalsAtOneTrace
						+ t];
				(*signalsForThisTrace)[idToWrite] = signal_value;
			}
			

			CoordinatesInCube coords(inlineNum, crosslineNum);
			auto newSeismPoint
				= new Seismic3DPointWithNeighboursAndSimilarityValues(
					coords, 
					signalsForThisTrace, 
					minId, 
					canCorrelateThisTrace
			);
			if(crosslineNum == _mainTraceCoordByY && inlineNum == _mainTraceCoordByX)
			{ 
				mainPoint = newSeismPoint;
			}
			allPoints->push_back(newSeismPoint);

			delete idsOfFixedPointsAtThisTrace;
		}
	}
	
	// корреляция опрной трассы со всеми остальными
	auto idsOfFixedPointsAtMainTrace = GetIdsOfFixedPointsForTraceWithCoordinates(
		_mainTraceCoordByX, 
		_mainTraceCoordByY, 
		mainPoint->GetIdWhereTraceStarts()
	);

	int32_t totalNeedToProcess = allPoints->size();
	double progressValue = 0 / (double)totalNeedToProcess;
	auto wasCancelled = _callback(progressValue, _callbackData);

	for(int i = 0; i < allPoints->size(); i++)
	{
		auto elem = allPoints->at(i);
		if(elem->getCoordinates().inlineId == _mainTraceCoordByX
			&& elem->getCoordinates().crosslineId == _mainTraceCoordByY)
		{
			continue; // саму с собой не нужно коррелировать
		}

		if(!elem->CanCorrelateThisPoint())
		{
			continue;
		}

		auto idsOfFixedPointsAtYTrace = GetIdsOfFixedPointsForTraceWithCoordinates(
			elem->getCoordinates().inlineId, 
			elem->getCoordinates().crosslineId,
			elem->GetIdWhereTraceStarts()
		);

		Seismic3DPointWithNeighboursAndSimilarityValues& arg1 = *mainPoint;
		const Seismic3DPointWithNeighboursAndSimilarityValues& arg2 = *elem;
		auto neighbourInlineIdToDebug = elem->getCoordinates().inlineId;
		auto neighbourCrosslineIdToDebug = elem->getCoordinates().crosslineId;

		const int32_t arg3 = _max_shift_point_idx;
		const int32_t arg4 = _countOfFixedBorders;
		const int16_t* arg5 = idsOfFixedPointsAtMainTrace->data();
		Point3DSimilarityHelper similarityValueHelper(arg1,
			arg2,
			arg3,
			arg4,
			arg5, // TODO: n.romanov сместить индексы фикс границ?
			idsOfFixedPointsAtYTrace->data(), // TODO: n.romanov здесь тоже сместить
			_fix_point_idx_r,
			_r_idx
		);

		auto similarityValue = similarityValueHelper.getSimilarityOfTwoPointsNotEqualsLenghtOfTraces();
		// отображение из опорной трассы на i-ю (начиная с arg1._idWhereTraceStarts)
		auto mapping = similarityValueHelper.GetMappingFirstTraceOnSecond();
		// отображение с 0 id до _numOfSignalsAtOneTrace-1
		auto normalMapping = new std::vector<int16_t>(_numOfSignalsAtOneTrace, -32768);
		for(int j = arg1.GetIdWhereTraceStarts(), k = 0; k < mapping.size(); k++, j++)
		{
			normalMapping->operator[](j) = mapping[k];
		}

		auto neighbour = new Seismic3DPointNeighbour(arg2, similarityValue, *normalMapping);
		delete normalMapping;
		arg1.addNeighbor(neighbour);

		delete idsOfFixedPointsAtYTrace;

		progressValue = i / (double)totalNeedToProcess;
		auto wasCancelled = _callback(progressValue, _callbackData);

		if (wasCancelled)
		{
			return;
		}
	}

	// обратные корреляции мне не нужны, беру только из опорной точки
	// кординаты точек + отображения из опорной во все остальные

	HorizonWrapper horizons(_numOfSignalsAtOneTrace,
		_crosslineCount*_inlineCount,
		_crosslineCount,
		_inlineCount);
	
	// строим границы
	// TODO: n.romanov можно ускорить, если проходить заполнять не по очереди границ, а сразу целыми трассами
	// будет меньше get`ов и лишних проверок
	// начинаем с id, где начинается опорная трасса
	
	totalNeedToProcess = mainPoint->GetNumOfSignals() * _inlineCount * _crosslineCount;
	progressValue = 0 / (double)totalNeedToProcess;
	wasCancelled = _callback(progressValue, _callbackData);

	mainPoint->MakeDictionaryForNeighbours();

	const auto initialId = mainPoint->GetIdWhereTraceStarts();
	int horizonNum = 10; // номер текущей границы
	const int numOfPoint = _horizonsBreadth; // ширина одной границы
	int numOfPointFinished = 0;
	for(int signalId = initialId;
		signalId < initialId + mainPoint->GetNumOfSignals(); // обходим все сигналы опорной трассы (рисуем для них поверхности границ)
		)
	{
		for(int il = 0; il < _inlineCount; il++)
		{
			for(int cl = 0; cl < _crosslineCount; cl++)
			{
				if(cl != _mainTraceCoordByY || il != _mainTraceCoordByX)
				{
					auto neighbour = mainPoint->TryToFindNeighborPointByCoordsFast(CoordinatesInCube(il, cl));
					if(neighbour == nullptr)
					{
						horizons.SetHorizon(il, cl, signalId, 0);
					}
					else
					{
						auto mapping = neighbour->GetMappingSomeTraceOnThisNeighbor();
						auto mappedToId = mapping[signalId]; // сюда отобразилась опорная трасса в сигнале с id (signalId)
						if (mappedToId >= 0) // если есть отображение
						{
							auto neighbourStartId = neighbour->GetPoint().GetIdWhereTraceStarts();
							horizons.SetHorizon(il, cl, neighbourStartId + mappedToId, horizonNum);
						}
					}
				}
				else
				{
					horizons.SetHorizon(il, cl, signalId, horizonNum * 100);
				}

				progressValue = ((signalId-initialId) * _crosslineCount * _inlineCount + il * _crosslineCount + cl)
					/ (double)totalNeedToProcess;
				auto wasCancelled = _callback(progressValue, _callbackData);

				if (wasCancelled)
				{
					return;
				}
			}
		}

		numOfPointFinished++;
		if(numOfPointFinished >= numOfPoint)
		{
			numOfPointFinished = 0;
			signalId += _spaceBetweenHorizons; // space between horizons
			if(horizonNum > 0)
				horizonNum += 10;
			else
				horizonNum -= 10;

			horizonNum *= -1;
		}
		else
		{
			signalId++;
		}
	}

	// сохраняем поверхности с границами
	_horizonsDataOut = horizons.GetHorizonsData();

	// чистим память
	delete idsOfFixedPointsAtMainTrace;
	for (int i = 0; i < allPoints->size(); i++)
	{
		delete allPoints->at(i);
	}
	delete allPoints;
	
}

int32_t* MainAndOtherPointsData::GetHorizonsDataOut() const
{
	return _horizonsDataOut;
}

const std::vector<int16_t>* MainAndOtherPointsData::GetIdsOfFixedPointsForTraceWithCoordinates(
	int32_t inlineNum, 
	int32_t crosslineNum,
	int32_t min_id) const
{
	auto result = new std::vector<int16_t>(_countOfFixedBorders);
	for (int horizonNum = 0; horizonNum < _countOfFixedBorders; horizonNum++)
	{
		int32_t id = inlineNum
			+ _inlineCount * crosslineNum
			+ _inlineCount * _crosslineCount * horizonNum;
		result->operator[](horizonNum) = _surface_idx[id] - min_id;
	}

	return result;
}

// заполнение границ, которые не выставили, интерполяция
void MainAndOtherPointsData::FillEmptyTraces(const std::vector<int16_t>* mainTraceBorders)
{
	int maxValue = _inlineCount * _crosslineCount * _countOfFixedBorders;

	for(int il = 0; il < _inlineCount; il++)
	{
		for(int cl = 0; cl < _crosslineCount; cl++)
		{
			for (int horizonNum = 0; horizonNum < _countOfFixedBorders; horizonNum++)
			{
				int32_t id = il
					+ _inlineCount * cl
					+ _inlineCount * _crosslineCount * horizonNum;
				if(_surface_idx[id] <= -1) // не определена
				{
					int32_t neighbourId = (il-1)
						+ _inlineCount * cl
						+ _inlineCount * _crosslineCount * horizonNum;
					int32_t neighbourIdNext = (il + 1)
						+ _inlineCount * cl
						+ _inlineCount * _crosslineCount * horizonNum;
					if(neighbourId < 0 || neighbourId > maxValue || neighbourIdNext > maxValue || neighbourIdNext < 0)
					{
						_surface_idx[id] = mainTraceBorders->operator[](horizonNum);
					}
					else 
					{
						int32_t avg = (_surface_idx[neighbourIdNext] - _surface_idx[neighbourId]) / 2 + _surface_idx[neighbourId];
						_surface_idx[id] = _surface_idx[neighbourId];
					}
				}
				else if(_surface_idx[id] >= _numOfSignalsAtOneTrace) // выходит за пределы выборки
				{
					_surface_idx[id] = _numOfSignalsAtOneTrace - 1; // последний возможный сигнал
				}
			}
		}
	}
}
