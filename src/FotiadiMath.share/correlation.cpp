#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include <memory>
#include <fstream>
#include <numeric>
#include "fotiadi_math.h"
#include "correlation_alg.h"
#include "wave_alg.h"
#include "basic_alg.h"
#include "std_alg.h"


template<typename INT_T, typename FLOAT_T>
FLOAT_T MetricFunctionSimpleConvolutionTempl(
	const INT_T n_signal_x,
	const FLOAT_T signal_x[],
	const INT_T idx_x,
	const INT_T n_signal_y,
	const FLOAT_T signal_y[],
	const INT_T idx_y,
	const void* data
) {
	const INT_T r_idx = *(INT_T*)data;
	FLOAT_T sum = 0;
	INT_T counter = 0;
	for (INT_T i = -r_idx; i <= r_idx; i++) {
		const auto cur_idx_x = idx_x + i;
		const auto cur_idx_y = idx_y + i;
		if ((0 <= cur_idx_x && cur_idx_x < n_signal_x) && (0 <= cur_idx_y && cur_idx_y < n_signal_y)) {
			const auto cur_add = signal_x[cur_idx_x] * signal_y[cur_idx_y];
			if (!std::isnan(cur_add)) {
				sum += cur_add;
				counter++;
			}
		}
	}
	#ifdef _DEBUG
	const FLOAT_T min_count = (r_idx * 2) / 3.0 - 0.5;
	#endif
	if (0 == counter || counter < r_idx) {
		return std::numeric_limits<FLOAT_T>::quiet_NaN();
	}
	auto res = sgn(sum) * std::sqrt(std::abs(sum) / counter);
	return res;
}

double MetricFunctionSimpleConvolution(
	const int32_t n_signal_x,
	const double signal_x[],
	const int32_t idx_x,
	const int32_t n_signal_y,
	const double signal_y[],
	const int32_t idx_y,
	const void* data
) {
	return MetricFunctionSimpleConvolutionTempl<int32_t, double>(n_signal_x,
		signal_x, idx_x, n_signal_y, signal_y, idx_y, data);
}

double MetricFunctionSimpleConvolutionDouble(
	const int16_t n_signal_x,
	const double signal_x[],
	const int16_t idx_x,
	const int16_t n_signal_y,
	const double signal_y[],
	const int16_t idx_y,
	const void* data
) {
	return MetricFunctionSimpleConvolutionTempl<int16_t, double>(n_signal_x,
		signal_x, idx_x, n_signal_y, signal_y, idx_y, data);
}

int32_t  FM_CorrelationByWaveAlgorithm(
	const int32_t n_grid_x,				//!< количество сигналов на сетке X 
	const int32_t shift_idx_x,			//!< смещение индекса в сетке X до начала интервала
	const double signal_x[],			//!< сигналы на сетке X	[n_grid_x]
	const int32_t n_grid_y,				//!< количество сигналов на сетке Y 
	const int32_t shift_idx_y,			//!< смещение индекса
	const double signal_y[],			//!< сигналы на сетке Y	[n_grid_y]
	const int32_t n_interval,			//!< количество точек в коррелируемом интервале
	const int32_t max_shift_point_idx,	//!< Максимальный сдвиг каждой точки диагонали по индексам
	const int32_t n_fix_point,          //!< Количество зафиксированных точек, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const int32_t fix_point_idx_x[],
	const int32_t fix_point_idx_y[],
	const int32_t fix_point_idx_r[],    //!< для каждой точки задается окрестность в которой точку можно сдвинуть
	const MetricFunction fm,			//!< Функция метрики 
	const void* pdata_fm,				//!< Данные для функции метрики 
	double grid_y_by_x[],               //!< [out] значение индексов сетки Y по сетки X [n_interval]. Может быть отображение, выходящее за границу массива, это нормальное поведение.
	double& target_function				//!< [out] Значение целевой функции для маршрута. 
) {
	//Размеры должны быть положительными
	if (n_grid_x <= 0 || n_grid_y <= 0 || shift_idx_x<0 || shift_idx_y<0 || n_interval<=0 || n_fix_point<0) {
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}
	//Интервалы должны находиться внутри массива
	if (n_grid_x< shift_idx_x+ n_interval || n_grid_y < shift_idx_y + n_interval) {
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}
	//Если существуют фиксированые точки, то массивы с индексами должны быть не нулевые 
	if (n_fix_point > 0 && (NULL == fix_point_idx_x || NULL == fix_point_idx_y || NULL == fix_point_idx_r)) {
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}
	//Проверка корректности фиксированных точек
	for (int32_t i = 0; i < n_fix_point; i++) {
		if (fix_point_idx_r[i]<0) {
			return FM_ERROR_PARAMETRS_NOT_VALID;
		}
		const auto idx_x = fix_point_idx_x[i] - shift_idx_x;
		const auto idx_y = fix_point_idx_y[i] - shift_idx_y;
		if (std::abs(idx_x - idx_y) > max_shift_point_idx) {
			return FM_ERROR_PARAMETRS_NOT_VALID;
		}
	}

	const int32_t n = (2 * max_shift_point_idx + 1);
	const int32_t m = n_interval;
	MatrixMazeDouble grid(n,m,0.0);
	//grid.for_all_change(
	grid.for_all_change_parallel(
		[&](int32_t i,int32_t j,double cur_val) -> double
		{
			const auto cur_fm = fm(n_grid_x, signal_x, shift_idx_x+j , n_grid_y, signal_y, shift_idx_y + j + i-max_shift_point_idx, pdata_fm);
			return cur_fm;
		}
	);
	//!Убираем невозможные точки 
	for (int32_t k = 0; k < n_fix_point; k++) {
		const auto idx_j = fix_point_idx_x[k]- shift_idx_x;
		const auto idx_i = (fix_point_idx_y[k] - shift_idx_y - idx_j) + max_shift_point_idx;
		for (int i = 0; i < n; i++) {
			if (!(std::abs(i - idx_i)<= fix_point_idx_r[k])) {
				grid(i, idx_j) = std::numeric_limits<double>::quiet_NaN();
			}
		}
	}
	//std::ofstream out("output.txt");
	//grid.print(out);
	//out.close();
	NormMatrix(grid);
	std::ofstream out2("output_norm.txt");
	grid.print(out2);
	out2.close();

	std::list<PointMazeDouble> l_starts;
	std::list<PointMazeDouble> l_ends;
	for (int32_t i = 0; i < n; i++) {
		const auto cur_point_start = PointMazeDouble(i, 0);
		if (!std::isnan(grid[cur_point_start])) {
			l_starts.push_back(cur_point_start);
		}
		const auto cur_point_end= PointMazeDouble(i, m - 1);
		if (!std::isnan(grid[cur_point_end])) {
			l_ends.push_back(cur_point_end);
		}
	}
	std::vector<PointMazeDouble> starts(l_starts.cbegin(),l_starts.cend());
	std::vector<PointMazeDouble> ends(l_ends.cbegin(), l_ends.cend());

	if (starts.size() == 0 || ends.size() == 0) {
		for (int32_t i = 0; i < n_interval; i++) {
			grid_y_by_x[i]= std::numeric_limits<double>::quiet_NaN();
		}
		return FM_ERROR_NO_SOLUTION_FOUND;
	}

	std::list< PointMazeDouble> path;
	//veAlgMazeMultiPoints(grid, starts, ends, NextStepFunctionRightUpDown, nullptr, path);
	target_function=WaveAlgMazeMultiPointsParallel(grid, starts, ends, NextStepFunctionRightUpDown, nullptr, path);
	target_function /= n_interval;
	for (int32_t i = 0; i < grid.cols(); i++) {
		std::list <PointMazeDouble> points;
		for (const auto cur_point_in_path : path) {
			if (cur_point_in_path.col() == i) {
				points.push_back(cur_point_in_path);
			}
		}
		double sum = 0;
		for (const auto cur_point : points) {
			sum += cur_point.row();
		}
		const double idx = shift_idx_y + i + (sum / points.size()) - max_shift_point_idx;
		grid_y_by_x[i]= idx;
	}
	//for (const auto cur_point : path) {
	//	grid[cur_point] = -1;
	//}
	//std::ofstream out3("output_result.txt");
	//grid.print(out3);
	//out3.close();

	return FM_ERROR_NO;
}

int32_t  FM_RecoverFunctionWithFilterWindowAndLineInterpolationByIndex(
	const int32_t n_grid_y,				 
	const double signal_y[],			
	const int32_t n_interval,			 
	const double grid_y_by_x[],			
	const int32_t r_idx_window,			
	double signal_y_by_grid_x[]			
) {
	std::vector<double> grid_y_by_x_filtered(n_interval,0);
	FilterWindowAvg(n_interval, grid_y_by_x, r_idx_window, grid_y_by_x_filtered.data());
	for (int32_t i = 0; i < n_interval; i++) {
		const double cur_idx = grid_y_by_x_filtered[i];
		double val = 0;
		if (cur_idx <= 0) {
			signal_y_by_grid_x[i] = signal_y[0];
			continue;
		}
		if (cur_idx >= n_grid_y-1) {
			signal_y_by_grid_x[i] = signal_y[n_grid_y - 1];
			continue;
		}
		const int64_t cur_midx = (int64_t)cur_idx;
		signal_y_by_grid_x[i] = signal_y[cur_midx] * (1-(cur_idx - cur_midx)) + signal_y[cur_midx + 1] * (1-(cur_midx + 1 - cur_idx));
	}
	return FM_ERROR_NO;
}
