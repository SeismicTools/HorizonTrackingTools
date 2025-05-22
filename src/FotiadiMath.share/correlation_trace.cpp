#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <limits>
#include <cmath>
#include <memory>

#include <fstream>

#include <numeric>
#include "correlation_alg.h"
#include "wave_alg.h"
#include "wave_alg_list_maze.h"
#include "basic_alg.h"
#include "std_alg.h"
#include "fotiadi_math_private.h"
#include "new_list_maze.h"

template <typename INT_T, typename FLOAT_T>
FLOAT_T MetricFunctionConvolutionWithShiftTempl(
	const INT_T n_signal_x,
	const FLOAT_T signal_x[],
	const INT_T idx_x,
	const INT_T n_signal_y,
	const FLOAT_T signal_y[],
	const INT_T idx_y,
	const void* data
) {
	const auto cdata = (const DataForMetricConvolutionWithShift*)data;
	FLOAT_T sum = 0;
	INT_T counter = 0;
	for (INT_T i = -cdata->r_idx; i <= cdata->r_idx; i++) {
		const auto cur_idx_x = idx_x + i;
		const auto cur_idx_y = idx_y + i;
		if ((0 <= cur_idx_x && cur_idx_x < n_signal_x) && (0 <= cur_idx_y && cur_idx_y < n_signal_y)) {
			const auto cur_add = (cdata->shift_signal_x + signal_x[cur_idx_x]) * (cdata->shift_signal_y + signal_y[cur_idx_y]);
			if (!std::isnan(cur_add)) {
				sum += cur_add;
				counter++;
			}
		}
	}
	const FLOAT_T min_count = cdata->r_idx * cdata->k_min_count - 0.5;
	if (0 == counter || counter < min_count) {
		return std::numeric_limits<FLOAT_T>::quiet_NaN();
	}
	auto res = sgn(sum) * std::sqrt(std::abs(sum) / counter);
	return res;
}

double MetricFunctionConvolutionWithShift(
	const int32_t n_signal_x,
	const double signal_x[],
	const int32_t idx_x,
	const int32_t n_signal_y,
	const double signal_y[],
	const int32_t idx_y,
	const void* data
) {
	return MetricFunctionConvolutionWithShiftTempl<int32_t, double>(n_signal_x,
		signal_x, idx_x, n_signal_y, signal_y, idx_y, data);
}

double MetricFunctionConvolutionWithShiftDouble(
	const int16_t n_signal_x,
	const double signal_x[],
	const int16_t idx_x,
	const int16_t n_signal_y,
	const double signal_y[],
	const int16_t idx_y,
	const void* data
)
{
	return MetricFunctionConvolutionWithShiftTempl<int16_t, double>(n_signal_x,
		signal_x, idx_x, n_signal_y, signal_y, idx_y, data);
}

int32_t FM_CorrelationByWaveNormalization(
	const uint64_t size,
	double data[],
	const double min_out,
	const double max_out
) {
	const auto interval_out = max_out - min_out;
	double min_in, max_in;
	array_interval_parallel<double>(size,data, min_in, max_in);
	const auto interval_in = max_in - min_in;
	//_parallel
	array_change_parallel<double>(size, data,
		[&](double cur_val) -> double
		{
			const auto cur_fm = ((cur_val-min_in)/interval_in)*interval_out+min_out;
			return cur_fm;
		}
	);
	return FM_ERROR_NO;
}

// реализация корреляции 2 трасс через хранение только значащей части матрицы new_list_maze.h
template <typename INT_T, typename FLOAT_T>
int32_t  FM_CorrelationByWaveAlgorithmForTraceTemplNew(
	//!< количество сигналов на сетке X
	const INT_T n_grid,
	//!< сигналы на сетке X	[n_grid]
	const FLOAT_T signal_x[],
	//!< сигналы на сетке Y	[n_grid]
	const FLOAT_T signal_y[],
	//!< Максимальный сдвиг каждой точки по индексам
	const INT_T max_shift_point_idx,
	//!< Количество зафиксированных точек, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const INT_T n_fix_point,
	const INT_T fix_point_idx_x[],
	const INT_T fix_point_idx_y[],
	//!< для каждой точки задается окрестность в которой точку можно сдвинуть
	const INT_T fix_point_idx_r[],
	//!< Функция метрики 
	const MetricFunctionTempl<FLOAT_T, INT_T> fm,
	//!< Данные для функции метрики 
	const void* pdata_fm,
	//!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
	INT_T x2y[],
	//!< [out] значение индексов сетки X по сетки Y [n_interval]. Для каждой точки X выставлено значение по Y.
	INT_T y2x[],
	//!< [out] Значение целевой функции для маршрута. 
	FLOAT_T& target_function
)
{
	// заполнение нулями и NaN`ами происходит автоматически
	NewListMaze<FLOAT_T> gridNew(max_shift_point_idx, n_grid, n_grid);

	// рассчитывает для каждого значащего значения матрицы функцию метрики
	gridNew.for_all_change_parallel(
		[&](const INT_T i, const INT_T j, const FLOAT_T cur_val) -> FLOAT_T
		{
			auto cur_fm = cur_val;
			if (!std::isnan(cur_val)) {
				cur_fm = fm(n_grid, signal_x, j, n_grid, signal_y, i, pdata_fm);
			}
			return cur_fm;
		}
	);

	const auto norm_value = NormMatrixList<FLOAT_T>(gridNew);

	//Стартовые точки
	std::list<PointMaze<INT_T>> l_starts;
	std::list<PointMaze<INT_T>> l_ends;
	auto maxShift = max_shift_point_idx;
	if (maxShift > gridNew.cols() || maxShift > gridNew.rows())
	{
		maxShift = std::min(gridNew.cols(), gridNew.rows())-1;
	}
	for (int32_t i = 0; i <= maxShift; i++) {
		const auto cur_point_start_x = PointMaze<INT_T>(0, i);
		if (!std::isnan(gridNew[cur_point_start_x])) {
			l_starts.push_back(cur_point_start_x);
		}
		if (i != 0) {
			const auto cur_point_start_y = PointMaze<INT_T>(i, 0);
			if (!std::isnan(gridNew[cur_point_start_y])) {
				l_starts.push_back(cur_point_start_y);
			}
		}
		const auto cur_point_end_x = PointMaze<INT_T>(n_grid - 1, n_grid - 1 - i);
		if (!std::isnan(gridNew[cur_point_end_x])) {
			l_ends.push_back(cur_point_end_x);
		}
		if (i != 0) {
			const auto cur_point_end_y = PointMaze<INT_T>(n_grid - 1 - i, n_grid - 1);
			if (!std::isnan(gridNew[cur_point_end_y])) {
				l_ends.push_back(cur_point_end_y);
			}
		}
	}
	std::vector<PointMaze<INT_T>> starts(l_starts.cbegin(), l_starts.cend());
	std::vector<PointMaze<INT_T>> ends(l_ends.cbegin(), l_ends.cend());

	std::list< PointMaze<INT_T>> path;
	target_function = WaveAlgMazeMultiPointsParallelTemplNewListMaze<INT_T, FLOAT_T>
		(gridNew, starts, ends, NextStepFunctionRightDownTempl<INT_T, FLOAT_T>, nullptr, path, max_shift_point_idx);
	target_function /= path.size();
	target_function -= norm_value;
	for (int32_t i = 0; i < n_grid; i++) {
		x2y[i] = std::numeric_limits<INT_T>::min();
		y2x[i] = std::numeric_limits<INT_T>::min();
	}
	for (const auto& cur_point : path) {
		x2y[cur_point.col()] = cur_point.row();
		y2x[cur_point.row()] = cur_point.col();
	}
	return FM_ERROR_NO;
}

// TODO: n.romanov на текущий момент не поддерживает радиус фиксации точек fix_point_idx_r
/// <summary>
/// Считает похожесть двух трасс
/// </summary>
template <typename INT_T, typename FLOAT_T>
int32_t  FM_CorrelationByWaveAlgorithmForTraceTempl(
	//!< количество сигналов на сетке X
	const INT_T n_grid, 
	//!< сигналы на сетке X	[n_grid]
	const FLOAT_T signal_x[],	
	//!< сигналы на сетке Y	[n_grid]
	const FLOAT_T signal_y[],	
	//!< Максимальный сдвиг каждой точки по индексам
	const INT_T max_shift_point_idx,	
	//!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const INT_T n_fix_borders,								
	const INT_T fix_point_idx_x[],						
	const INT_T fix_point_idx_y[],	
	//!< для каждой точки задается окрестность в которой точку можно сдвинуть
	const INT_T fix_point_idx_r[],	
	//!< Функция метрики 
	const MetricFunctionTempl<FLOAT_T, INT_T> fm,		
	//!< Данные для функции метрики 
	const void* pdata_fm,
	//!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
	INT_T x2y[],							
	//!< [out] значение индексов сетки X по сетки Y [n_interval]. Для каждой точки X выставлено значение по Y.
	INT_T y2x[],	
	//!< [out] Значение целевой функции для маршрута. 
	FLOAT_T& target_function									
)
{
	MatrixMaze<INT_T, FLOAT_T> grid(n_grid, n_grid);

	grid.for_all_change_parallel(
		[&](const INT_T i, const INT_T j, const FLOAT_T cur_val) -> FLOAT_T
		{
			FLOAT_T cur_fm = std::numeric_limits<FLOAT_T>::quiet_NaN(); // значения за диагональю равны null
			if (std::abs(i - j) <= max_shift_point_idx && !std::isnan(grid(i, j)))
			{
				bool f = true;

				for (INT_T k = 0; k < n_fix_borders; k++)
				{
					if(i == fix_point_idx_y[k] && j == fix_point_idx_x[k])
					{
						// заполнение верхнего правого квадрата
						auto i1 = 0;
						auto j1 = j;
						for(; i1 <= i;)
						{
							grid.set_value(i1, j1, cur_fm);
							j1++;
							if(j1 == n_grid)
							{
								j1 = j+1;
								i1++;
							}
						}
						// заполнение нижнего левого квадрата
						i1 = i + 1;
						j1 = 0;
						for(; i1 < n_grid;)
						{
							grid.set_value(i1, j1, cur_fm);
							j1++;
							if(j1 > j)
							{
								j1 = 0;
								i1++;
							}
						}
					}
				}
				if (f)
					cur_fm = 0.0; // для фиксированной точки + значения на диагонали = 0
			}
			return cur_fm;
		}
	);

	// рассчет значений похожести
	grid.for_all_change_parallel(
		[&](const INT_T i, const INT_T j, const FLOAT_T cur_val) -> FLOAT_T
		{
			auto cur_fm = cur_val;
			if (!std::isnan(cur_val)) {
				cur_fm = fm(n_grid, signal_x, j, n_grid, signal_y, i, pdata_fm);
			}
			return cur_fm;

		}
	);

	#ifdef _DEBUG
	// пишет в файл, только если его нет
	std::string folderPath = "./plugins/fotiadi_math_internal/saved_traces/";
	std::string fileNameForTrace = folderPath + std::string("matrix_save") + std::string(".txt");
	if (FILE* file = fopen(fileNameForTrace.c_str(), "r")) {
		fclose(file);
	}
	else {
		std::ofstream fileToWrite(fileNameForTrace.c_str());
		grid.print(fileToWrite);
		fileToWrite.close();
	}
	#endif

	const auto norm_value = NormMatrix<INT_T, FLOAT_T>(grid);

	//Стартовые точки
	std::list<PointMaze<INT_T>> l_starts;
	std::list<PointMaze<INT_T>> l_ends;
	auto maxShift = max_shift_point_idx;
	if(maxShift > grid.cols() || maxShift > grid.rows())
	{
		maxShift = std::min(grid.cols(), grid.rows())-1;
	}
	for (int32_t i = 0; i <= maxShift; i++) {
		const auto cur_point_start_x = PointMaze<INT_T>(0, i);
		if (!std::isnan(grid[cur_point_start_x])) {
			l_starts.push_back(cur_point_start_x);
		}
		if (i != 0) {
			const auto cur_point_start_y = PointMaze<INT_T>(i, 0);
			if (!std::isnan(grid[cur_point_start_y])) {
				l_starts.push_back(cur_point_start_y);
			}
		}
		const auto cur_point_end_x = PointMaze<INT_T>(n_grid - 1, n_grid - 1 - i);
		if (!std::isnan(grid[cur_point_end_x])) {
			l_ends.push_back(cur_point_end_x);
		}
		if (i != 0) {
			const auto cur_point_end_y = PointMaze<INT_T>(n_grid - 1 - i, n_grid - 1);
			if (!std::isnan(grid[cur_point_end_y])) {
				l_ends.push_back(cur_point_end_y);
			}
		}
	}
	std::vector<PointMaze<INT_T>> starts(l_starts.cbegin(), l_starts.cend());
	std::vector<PointMaze<INT_T>> ends(l_ends.cbegin(), l_ends.cend());

	std::list< PointMaze<INT_T>> path;
	target_function = WaveAlgMazeMultiPointsParallelTempl<INT_T, FLOAT_T>(grid, starts, ends, NextStepFunctionRightDownTempl<INT_T, FLOAT_T>, nullptr, path);
	target_function /= path.size();
	target_function -= norm_value;
	for (int32_t i = 0; i < n_grid; i++) {
		x2y[i] = std::numeric_limits<INT_T>::min();
		y2x[i] = std::numeric_limits<INT_T>::min();
	}
	for (const auto& cur_point : path) {
		x2y[cur_point.col()] = cur_point.row();
		y2x[cur_point.row()] = cur_point.col();
	}
	return FM_ERROR_NO;
}

/// <summary>
/// Считает похожесть двух трасс
/// </summary>
int32_t  FM_CorrelationByWaveAlgorithmForTrace(
	const int16_t n_grid,									//!< количество сигналов на сетке X 
	const double signal_x[],								//!< сигналы на сетке X	[n_grid]
	const double signal_y[],								//!< сигналы на сетке Y	[n_grid]
	const int16_t max_shift_point_idx,						//!< Максимальный сдвиг каждой точки по индексам
	const int16_t n_fix_borders,						    //!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const int16_t idsOfFixedPointsAtXTrace[],				//!< число фиксированных точек на одну трассу = число границ
	const int16_t idsOfFixedPointsAtYTrace[],				//!< 
	const int16_t fix_point_idx_r[],						//!< для каждой точки задается окрестность в которой точку можно сдвинуть
	const MetricFunctionDouble fm,							//!< Функция метрики 
	const void* pdata_fm,									//!< Данные для функции метрики 
	int16_t x2y[],											//!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
	int16_t y2x[],											//!< [out] значение индексов сетки X по сетки Y [n_interval]. Для каждой точки X выставлено значение по Y.
	double& target_function									//!< [out] Значение целевой функции для маршрута. 
) {
	MetricFunctionTempl<double, int16_t> funcPtr = fm;
	return FM_CorrelationByWaveAlgorithmForTraceTempl<int16_t, double>(n_grid,
		signal_x, signal_y, max_shift_point_idx, n_fix_borders, idsOfFixedPointsAtXTrace,
		idsOfFixedPointsAtYTrace, fix_point_idx_r, funcPtr, pdata_fm, x2y, y2x, target_function);
}

// реализация FM_CorrelationByWaveAlgorithmForTrace через новый list maze
static int32_t  FM_CorrelationByWaveAlgorithmForTraceDoubleNewListMaze(
	const int16_t n_grid,									//!< количество сигналов на сетке X 
	const double signal_x[],								//!< сигналы на сетке X	[n_grid]
	const double signal_y[],								//!< сигналы на сетке Y	[n_grid]
	const int16_t max_shift_point_idx,						//!< Максимальный сдвиг каждой точки по индексам
	const int16_t n_fix_point,								//!< Количество зафиксированных точек, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const int16_t fix_point_idx_x[],						//!< 
	const int16_t fix_point_idx_y[],						//!< 
	const int16_t fix_point_idx_r[],						//!< для каждой точки задается окрестность в которой точку можно сдвинуть
	const MetricFunctionDouble fm,								//!< Функция метрики 
	const void* pdata_fm,									//!< Данные для функции метрики 
	int16_t x2y[],											//!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
	int16_t y2x[],											//!< [out] значение индексов сетки X по сетки Y [n_interval]. Для каждой точки X выставлено значение по Y.
	double& target_function									//!< [out] Значение целевой функции для маршрута. 
) {
	MetricFunctionTempl<double, int16_t> funcPtr = fm;
	return FM_CorrelationByWaveAlgorithmForTraceTemplNew<int16_t, double>(n_grid,
		signal_x, signal_y, max_shift_point_idx, n_fix_point, fix_point_idx_x,
		fix_point_idx_y, fix_point_idx_r, funcPtr, pdata_fm, x2y, y2x, target_function);
}

int32_t FM_CorrelationByWaveAlgorithmForTraceFloat(
	const uint16_t n_grid,									//!< количество сигналов на сетке X 
	const float signal_x[],									//!< сигналы на сетке X	[n_grid]
	const float signal_y[],									//!< сигналы на сетке Y	[n_grid]
	const uint16_t max_shift_point_idx,						//!< Максимальный сдвиг каждой точки по индексам
	const uint16_t n_fix_point,								//!< Количество зафиксированных точек, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const uint16_t fix_point_idx_x[],						//!< 
	const uint16_t fix_point_idx_y[],						//!< 
	const uint16_t fix_point_idx_r[],						//!< для каждой точки задается окрестность в которой точку можно сдвинуть
	const MetricFunctionFloat fm,							//!< Функция метрики 
	const void* pdata_fm,									//!< Данные для функции метрики 
	uint16_t x2y[],											//!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
	uint16_t y2x[],											//!< [out] значение индексов сетки X по сетки Y [n_interval]. Для каждой точки X выставлено значение по Y.
	float& target_function									//!< [out] Значение целевой функции для маршрута. 
)
{
	return FM_CorrelationByWaveAlgorithmForTraceTempl<uint16_t, float>(n_grid,
		signal_x, signal_y, max_shift_point_idx, n_fix_point, fix_point_idx_x,
		fix_point_idx_y, fix_point_idx_r, fm, pdata_fm, x2y, y2x, target_function);
}
