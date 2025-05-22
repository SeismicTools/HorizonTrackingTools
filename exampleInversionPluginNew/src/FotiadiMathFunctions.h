#pragma once

#include <string>


class FotiadiMathFunctions
{
	public:
		typedef int32_t(*FM_TestFunc)(const int32_t, std::string&);
		typedef int32_t(*CallBackProgress)(const double progress);
		typedef int32_t(*setPorgressValueFunc)(const double value, const void* data);
		typedef int32_t(*FM_Seismic3DCorrelation)
			(
				const int32_t inlineCount,
				const int32_t crosslineCount,
				const int32_t numOfSignalsAtOneTrace,
				const double allSignals[],
				const int32_t max_shift_point_idx,
				const int32_t countOfFixedBorders,
				int16_t surface_idx[],
				const int16_t fix_point_idx_r[], // по умолчанию 0
				const void* data, // тут будет указатель на объект класса PluginProgressReporterInterface
				setPorgressValueFunc callback,
				int32_t r_idx,
				void** out_correlationData
				);
		typedef int32_t(*FM_Seismic3DCorrelation_one_to_many)
			(
				const int32_t inlineCount,
				const int32_t crosslineCount,
				const int32_t numOfSignalsAtOneTrace,
				const double allSignals[],
				const int32_t max_shift_point_idx,
				const int32_t countOfFixedBorders,
				int16_t surface_idx[],
				const int16_t fix_point_idx_r[],
				const void* callbackData,
				setPorgressValueFunc callback,
				int32_t r_idx,
				const int32_t mainTraceCoordByX,
				const int32_t mainTraceCoordByY,
				const int32_t horizonsBreadth,			// ширина границ
				const int32_t spaceBetweenHorizons,		// расстояние между границами
				void** out_correlationData
			);
		typedef int32_t(*FM_S3D_Free)(
			const void* savedObject
			);
		typedef int32_t(*FM_S3D_FreeArray)(
			const void* savedObjectWithCorrelationData
			);
		typedef int32_t(*FM_S3DDefineReflectingHorizonsForEntireTrace)(
			const void* savedObjectWithCorrelationData,
			const int32_t traceCoordByX,
			const int32_t traceCoordByY,
			int32_t** out_surfaceIds
			);
		typedef int32_t(*FM_S3DDefineReflectingHorizonsForEntireTraceNew)(
			const void* savedObjectWithCorrelationData,
			const int32_t traceCoordByX,
			const int32_t traceCoordByY,
			const int32_t countOfFixedBorders, 
			int16_t surface_idx[], 
			const int32_t spacesBetweenTwoHorizons, // расстояние между границами
			const int32_t sizeOfOneHorizon, 
			const void* callbackData, // тут будет указатель на объект класса PluginProgressReporterInterface
			setPorgressValueFunc callback,
			const int32_t horizonsBreadth,			// ширина границ
			int32_t** out_surfaceIds
		);
	
	private:
	
};
