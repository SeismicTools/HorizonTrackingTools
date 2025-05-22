#pragma once

#include <cstdint>
#include <TypedefStuff.h>
#include <vector>

class MainAndOtherPointsData
{
	public:
		MainAndOtherPointsData(
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
		);

		void CorrelateMainTraceWithAll();

		int32_t* GetHorizonsDataOut() const; 

	private:
		const int32_t _inlineCount;
		const int32_t _crosslineCount;
		const int32_t _numOfSignalsAtOneTrace;
		const double* _signals;
		const int32_t _max_shift_point_idx;
		const int32_t _countOfFixedBorders;
		int16_t* _surface_idx;
		const int16_t* _fix_point_idx_r;
		const void* _callbackData;
		setPorgressValueFunc _callback;
		int32_t _r_idx;
		const int32_t _mainTraceCoordByX;
		const int32_t _mainTraceCoordByY;
		int32_t* _horizonsDataOut = nullptr; // delete для неё делать через FM_S3D_FreeArray в seismic3D_Nikita
		const int32_t _horizonsBreadth;			// ширина границ
		const int32_t _spaceBetweenHorizons;		// расстояние между границами

		const std::vector<int16_t>* GetIdsOfFixedPointsForTraceWithCoordinates(
			int32_t inlineNum, 
			int32_t crosslineNum,
			int32_t min_id // число показывает, на сколько id от начала взяли сигналы (то-есть сигналы берутся не с нулевого, а с minId, из-за этого все индексы границ нужно смещать)
		) const;

		void FillEmptyTraces(const std::vector<int16_t>* mainTraceBorders);

};
