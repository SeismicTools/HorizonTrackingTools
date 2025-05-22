#pragma once

#include <cstdint>
#include "TypedefStuff.h"

class DataToFillGraphPoints {
	public:
		DataToFillGraphPoints(const int32_t& inlineCount, const int32_t& crosslineCount,
			const int32_t& n_time, const double* signals,
			const int32_t maxShiftAtEachPointByIndexBetweenNeighboringTraces,
			const int32_t countOfFixedBorders,
			int16_t* fixedPointsIdsForEachTrace,
			const int16_t* fix_point_idx_r,
			const int32_t& r_idx,
			setPorgressValueFunc callback,
			const void* callbackData)
				: _inlineCount(inlineCount), _crosslineCount(crosslineCount),
					_nTime(n_time),
					_signals(signals),
					_maxShiftAtEachPointByIndexBetweenNeighboringTraces(maxShiftAtEachPointByIndexBetweenNeighboringTraces),
					_countOfFixedBorders(countOfFixedBorders),
					_fixedPointsIdsForEachTrace(fixedPointsIdsForEachTrace),
					_fix_point_idx_r(fix_point_idx_r),
					_rIdx(r_idx),
					_callback(callback),
					_callbackData(callbackData)
		{
	
		}
	
		const int32_t GetNTraceY() const
		{
			return _inlineCount;
		}
	
		const int32_t GetNTraceX() const
		{
			return _crosslineCount;
		}
	
		const int32_t GetNTime() const
		{
			return _nTime;
		}
	
		const double* GetSignals() const
		{
			return _signals;
		}

		const int32_t& GetMaxShiftAtEachPointByIndexBetweenNeighboringTraces() const
		{
			return _maxShiftAtEachPointByIndexBetweenNeighboringTraces;
		}

		const int32_t& GetCountOfFixedBorders() const
		{
			return _countOfFixedBorders;
		}

		int16_t* GetFixedPointsIdsForEachTrace() const
		{
			return _fixedPointsIdsForEachTrace;
		}

		const int16_t* GetBordersRadiuses() const
		{
			return _fix_point_idx_r;
		}

		const int32_t& GetRIdx() const
		{
			return _rIdx;
		}

		//const std::vector<int16_t>* GetIdsOfFixedPointsForTraceWithCoordinates(int32_t inlineNum, int32_t crosslineNum) const
		//{
		//	auto result = new std::vector<int16_t>(_countOfFixedBorders);
		//	for(int horizonNum = 0; horizonNum < _countOfFixedBorders; horizonNum++)
		//	{
		//		int32_t id = inlineNum 
		//			+ _inlineCount * crosslineNum 
		//			+ _inlineCount * _crosslineCount * horizonNum;
		//		result->operator[](horizonNum) = _fixedPointsIdsForEachTrace[id];
		//	}

		//	return result;
		//}

		const std::vector<int16_t>* GetIdsOfFixedPointsForTraceWithCoordinatesWithOffset(
			int32_t inlineNum, 
			int32_t crosslineNum,
			int32_t min_id
		) const
		{
			auto result = new std::vector<int16_t>(_countOfFixedBorders);
			for (int horizonNum = 0; horizonNum < _countOfFixedBorders; horizonNum++)
			{
				int32_t id = inlineNum
					+ _inlineCount * crosslineNum
					+ _inlineCount * _crosslineCount * horizonNum;
				result->operator[](horizonNum) = _fixedPointsIdsForEachTrace[id] - min_id;
			}

			return result;
		}

		setPorgressValueFunc GetCallbackProgressReporter() const
		{
			return _callback;
		}

		const void* GetCallbackData() const
		{
			return _callbackData;
		}

	private:
		const int32_t _inlineCount;
		const int32_t _crosslineCount;
		const int32_t _nTime;
		const double* _signals;
		const int32_t _maxShiftAtEachPointByIndexBetweenNeighboringTraces;
		const int32_t _countOfFixedBorders;
		int16_t* _fixedPointsIdsForEachTrace; // массив размера [_nTraceY * _nTraceY * _countOfFixedBorders]
		const int16_t* _fix_point_idx_r; // радиус фиксации для каждоый точки каждой границы [_countOfFixedBorders]
		const int32_t& _rIdx;
		setPorgressValueFunc _callback;
		const void* _callbackData;
};
