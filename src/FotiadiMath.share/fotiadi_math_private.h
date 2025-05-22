#pragma once
#include "fotiadi_math.h"

extern "C" EXPORT_FM double MetricFunctionConvolutionWithShift(
	const int32_t n_signal_x,
	const double signal_x[],
	const int32_t idx_x,
	const int32_t n_signal_y,
	const double signal_y[],
	const int32_t idx_y,
	const void* data
);

/// <summary>
/// Индексы - int16_t
/// Значения - double
/// </summary>
/// <param name="n_signal_x"></param>
/// <param name="signal_x"></param>
/// <param name="idx_x"></param>
/// <param name="n_signal_y"></param>
/// <param name="signal_y"></param>
/// <param name="idx_y"></param>
/// <param name="data"></param>
/// <returns></returns>
extern "C" EXPORT_FM double MetricFunctionConvolutionWithShiftDouble(
	const int16_t n_signal_x,
	const double signal_x[],
	const int16_t idx_x,
	const int16_t n_signal_y,
	const double signal_y[],
	const int16_t idx_y,
	const void* data
);

//#pragma pack(1)
typedef struct {
	uint16_t r_idx;       //!< Радиус в индексах окресности для свертки 
	double k_min_count;	  //!< Коэффициент, для минимального вычисления функции - 2/3
	double shift_signal_x;  //!< Сдвиг сигнала для свертки
	double shift_signal_y;  //!< Сдвиг сигнала для свертки
}DataForMetricConvolutionWithShift;

extern "C" EXPORT_FM int32_t  FM_CorrelationByWaveAlgorithmForTraceDoubleNewListMaze(
	const int16_t n_grid,									//!< количество сигналов на сетке X 
	const double signal_x[],								//!< сигналы на сетке X	[n_grid]
	const double signal_y[],								//!< сигналы на сетке Y	[n_grid]
	const int16_t max_shift_point_idx,						//!< Максимальный сдвиг каждой точки по индексам
	const int16_t n_fix_point,								//!< Количество зафиксированных точек, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const int16_t fix_point_idx_x[],						//!< 
	const int16_t fix_point_idx_y[],						//!< 
	const int16_t fix_point_idx_r[],						//!< для каждой точки задается окрестность в которой точку можно сдвинуть
	const MetricFunctionDouble fm,							//!< Функция метрики 
	const void* pdata_fm,									//!< Данные для функции метрики 
	int16_t x2y[],											//!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
	int16_t y2x[],											//!< [out] значение индексов сетки X по сетки Y [n_interval]. Для каждой точки X выставлено значение по Y.
	double& target_function									//!< [out] Значение целевой функции для маршрута. 
);

extern "C" EXPORT_FM int32_t  FM_CorrelationByWaveAlgorithmForTraceFloat(
	const uint16_t n_grid,									//!< количество сигналов на сетке X 
	const float signal_x[],									//!< сигналы на сетке X	[n_grid]
	const float signal_y[],									//!< сигналы на сетке Y	[n_grid]
	const uint16_t max_shift_point_idx,						//!< Максимальный сдвиг каждой точки по индексам
	const uint16_t n_fix_point,								//!< Количество зафиксированных точек, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const uint16_t fix_point_idx_x[],						//!< 
	const uint16_t fix_point_idx_y[],						//!< 
	const uint16_t fix_point_idx_r[],						//!< для каждой точки задается окрестность в которой точку можно сдвинуть
	const MetricFunctionFloat fm,			//!< Функция метрики 
	const void* pdata_fm,									//!< Данные для функции метрики 
	uint16_t x2y[],											//!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
	uint16_t y2x[],											//!< [out] значение индексов сетки X по сетки Y [n_interval]. Для каждой точки X выставлено значение по Y.
	float& target_function									//!< [out] Значение целевой функции для маршрута. 
);

extern "C" EXPORT_FM int32_t FM_CorrelationByWaveNormalization(
	const uint64_t size,			
	double data[],				
	const double min,
	const double max
);

extern "C" EXPORT_FM int32_t FM_CorrelationByWaveAlgorithmForTrace(
	const int16_t n_grid,									//!< количество сигналов на сетке X 
	const double signal_x[],								//!< сигналы на сетке X	[n_grid]
	const double signal_y[],								//!< сигналы на сетке Y	[n_grid]
	const int16_t max_shift_point_idx,						//!< Максимальный сдвиг каждой точки по индексам
	const int16_t n_fix_borders,							//!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const int16_t idsOfFixedPointsAtXTrace[],				//!< число фиксированных точек на одну трассу = число границ
	const int16_t idsOfFixedPointsAtYTrace[],				//!< 
	const int16_t fix_point_idx_r[],						//!< [n_fix_borders] для каждой точки задается окрестность в которой точку можно сдвинуть
	const MetricFunctionDouble fm,							//!< Функция метрики 
	const void* pdata_fm,									//!< Данные для функции метрики 
	int16_t x2y[],											//!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
	int16_t y2x[],											//!< [out] значение индексов сетки X по сетки Y [n_interval]. Для каждой точки X выставлено значение по Y.
	double& target_function									//!< [out] Значение целевой функции для маршрута. 
);
