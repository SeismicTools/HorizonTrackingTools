#include <vector>
#include <list>
#include <algorithm>
#include "fotiadi_math.h"
#include "std_alg.h"
#include "basic_alg.h"
#include "fotiadi_util.h"
#include <string>

void CalcGrid(
	const double start_interval, 
	const double finish_interval,
	const double step,
	const double start_point,
	std::vector<double>& grid_out,
	int32_t& shift_idx_start
	) {
	std::list<double> grid;
	shift_idx_start = 0;
	grid.push_back(start_point);
	for (double cur_point = start_point - step; cur_point >= start_interval; cur_point -= step) {
		grid.push_front(cur_point);
		shift_idx_start++;
	}
	for (double cur_point = start_point + step; cur_point <= finish_interval; cur_point += step) {
		grid.push_back(cur_point);
	}
	grid_out.resize(grid.size());
	std::copy(grid.begin(),grid.end(), grid_out.begin());
}


int32_t  FM_PrepareDataForCorrelation(
	const int32_t n_signal_init_x,
	const double grid_init_x[],
	const double signal_init_x[],
	const int32_t n_signal_init_y,
	const double grid_init_y[],
	const double signal_init_y[],
	const int32_t n_fix_point_init,          //!< Количество зафиксированных точек, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const double fix_point_init_x[],
	const double fix_point_init_y[],
	const double fix_point_init_r[],
	const double start_interval,
	const double finish_interval,
	const double step_grid,
	const double max_shift,
	int32_t& n_interval,
	const int32_t n_signal_x,					//!< количество сигналов на сетке X 
	int32_t& n_signal_idx_x,
	int32_t& shift_idx_x,
	double signal_idx_x[],
	const int32_t n_signal_y,					//!< количество сигналов на сетке X 
	int32_t& n_signal_idx_y,
	int32_t& shift_idx_y,
	double signal_idx_y[],
	int32_t fix_point_idx_x[],
	int32_t fix_point_idx_y[],
	int32_t fix_point_idx_r[],
	int32_t& max_shift_point_idx
) {
	if (start_interval > finish_interval) {
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}
	if (start_interval < grid_init_x[0] || start_interval < grid_init_y[0]) {
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}
	if (grid_init_x[n_signal_init_x-1]< finish_interval || grid_init_y[n_signal_init_y - 1] < finish_interval) {
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}
	std::vector<double> grid_x, grid_y;
	//! Расчитываем сетку X
	CalcGrid(grid_init_x[0], grid_init_x[n_signal_init_x - 1], step_grid, start_interval, grid_x, shift_idx_x);
	if (grid_x.size() > n_signal_x) {
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}
	n_signal_idx_x = grid_x.size();
	LineTransformFunction(n_signal_init_x, grid_init_x, signal_init_x,n_signal_idx_x,grid_x.data(), signal_idx_x);
	//! Расчитываем сетку Y
	CalcGrid(grid_init_y[0], grid_init_y[n_signal_init_y - 1], step_grid, start_interval, grid_y, shift_idx_y);
	if (grid_y.size() > n_signal_y) {
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}
	n_signal_idx_y = grid_y.size();
	LineTransformFunction(n_signal_init_y, grid_init_y, signal_init_y, n_signal_idx_y, grid_y.data(), signal_idx_y);
	//! переводим точки в индексы
	for (int32_t i = 0; i < n_fix_point_init; i++) {
		const auto idx_x=binary_search_find_index(grid_x, fix_point_init_x[i]- step_grid / 2);
		if(idx_x<0) {
			return FM_ERROR_PARAMETRS_NOT_VALID;
		}
		fix_point_idx_x[i] = idx_x;

		const auto idx_y = binary_search_find_index(grid_y, fix_point_init_y[i] - step_grid / 2);
		if (idx_y < 0) {
			return FM_ERROR_PARAMETRS_NOT_VALID;
		}
		fix_point_idx_y[i] = idx_y;

		fix_point_idx_r[i] = (int32_t)((fix_point_init_r[i] + step_grid / 2) / step_grid);
	}
	
	n_interval = (int32_t)(0.5+(finish_interval - start_interval) / step_grid);
	max_shift_point_idx = (int32_t) (0.5 + (max_shift+ step_grid/2) / step_grid);
	return FM_ERROR_NO;
}

int32_t FM_TestFunc(const int in, std::string& out)
{
	out = "Dll was ran with text = " + std::to_string(in);
	return FM_ERROR_NO;
}

// подготовка трасс с одинаковой длиной
int32_t  FM_PrepareDataForCorrelationSeveral(
	const int32_t one_signal_size,			// число временных отсчетов одной трассы
	const int32_t n_trace,
	const double grid_init[],				// временные отсчеты [n_trace, one_signal_size]
	const double signal_init[],				// амплитуды для каждого значения времени [n_trace, one_signal_size]
	const int32_t n_fix_point_init,         //!< Количество зафиксированных точек, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const double fix_point_init[],			// зафиксированные точки для каждой из трасс [n_trace, one_signal_size]
	const double fix_point_init_r[],		// радиусы: их вот столько n_fix_point_init
	const double start_interval,
	const double finish_interval,
	const double step_grid,
	const double max_shift,
	int32_t& n_interval,
	const int32_t n_signal,				    //!< количество сигналов на каждой сетке
	int32_t& n_signal_idx,					//! [out] размер сетки (у всех трасс должен быть одинаковый) TODO: n.romanov, проверить, совпадает ли с one_signal_size
	double signal_idx[],					//! [out] [n_trace, one_signal_size]
	int32_t fix_point_idx[],				//! [out] фиксированные точки для всех трасс
	int32_t& shift_idx_x,
	double signal_idx_x[],
	const int32_t n_signal_y,					//!< количество сигналов на сетке X 
	int32_t& n_signal_idx_y,
	int32_t& shift_idx_y,
	double signal_idx_y[],
	int32_t fix_point_idx_r[],
	int32_t& max_shift_point_idx
) {
	if (start_interval > finish_interval) {
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}
	for(int i = 0; i < n_trace; i++)
	{
		if(start_interval < grid_init[one_signal_size*i])
		{
			return FM_ERROR_PARAMETRS_NOT_VALID;
		}
		if(grid_init[one_signal_size * (i+1) - 1] < finish_interval)
		{
			return FM_ERROR_PARAMETRS_NOT_VALID;
		}
	}

	for(int i = 0; i < n_trace; i++)
	{
		std::vector<double> grid;
		//! Расчитываем сетку
		CalcGrid(grid_init[i * one_signal_size + 0], grid_init[(i+1) * one_signal_size - 1], step_grid, start_interval, grid, shift_idx_x);
		if (grid.size() > one_signal_size) {
			return FM_ERROR_PARAMETRS_NOT_VALID;
		}
		n_signal_idx = grid.size();

		double* grid_init_sub = 0; // временные отсчеты для данной трассы
		GetIthArray<double>(grid_init, i, one_signal_size, grid_init_sub);

		double* signal_init_sub = 0; // амплитуды для данной трассы
		GetIthArray<double>(signal_init, i, one_signal_size, signal_init_sub);
		std::vector<double> signal_idx_x(n_signal_idx);
		LineTransformFunction(one_signal_size, grid_init_sub, signal_init_sub, n_signal_idx, grid.data(), signal_idx_x.data());
		FillIthArray(signal_idx, i, signal_idx_x.data(), n_signal_idx);

		//! переводим точки в индексы
		std::vector<int32_t> fix_point_idx_x(n_fix_point_init);
		for (int32_t j = 0; j < n_fix_point_init; j++) {
			double* fix_point_init_x = 0;
			GetIthArray(fix_point_init, i, one_signal_size, fix_point_init_x);
			const auto idx_x = binary_search_find_index(grid, fix_point_init_x[j] - step_grid / 2);
			if (idx_x < 0) {
				return FM_ERROR_PARAMETRS_NOT_VALID;
			}
			fix_point_idx_x[j] = idx_x;

			fix_point_idx_r[j] = (int32_t)((fix_point_init_r[j] + step_grid / 2) / step_grid);
		}
		FillIthArray<int32_t>(fix_point_idx, i, fix_point_idx_x.data(), n_fix_point_init);
	}

	n_interval = (int32_t)((finish_interval - start_interval) / step_grid);
	max_shift_point_idx = (int32_t)((max_shift + step_grid / 2) / step_grid);
	return FM_ERROR_NO;
}
