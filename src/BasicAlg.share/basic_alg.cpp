#include "basic_alg.h"
#include <vector>
#include <limits>
#include <cmath>

void FilterWindowAvg(const int32_t n, const double arr_init[], const int32_t r_window, double arr_filtered[]) {
	double sum = 0;
	int32_t count = 0;
	for (int32_t i = 0; i < r_window && i<n; i++) {
		if (!std::isnan(arr_init[i])){
			sum += arr_init[i];
			count++;
		}
	}
	for (int32_t i = 0; i < n; i++) {
		const auto idx_back = i - r_window-1; //Индекс значение которого нужно убрать из общей суммы
		if (idx_back >= 0 && !std::isnan(arr_init[idx_back])) {
			sum -= arr_init[idx_back];
			count--;
		}
		const auto idx_next = i + r_window;   //Индекс значение которого нужно добавить в общую сумму
		if (idx_next < n && !std::isnan(arr_init[idx_next])) {
			sum += arr_init[idx_next];
			count++;
		}
		arr_filtered[i] = count > 0 ? sum/count : std::numeric_limits<double>::quiet_NaN();
	}
}

double linef(const double x1, const double y1, const double x2, const double y2, const double x) {
	const auto y = ((x - x1) / (x2 - x1)) * (y2 - y1) + y1;
	return y;
}

void LineTransformFunction(const int32_t n, const double grid[], const double f[], const int32_t n_new, const double grid_new[], double f_new[]) 
{
	if (n == 0) {
		for (int32_t i = 0; i < n_new; i++) {
			f_new[i] = std::numeric_limits<double>::quiet_NaN();
		}
	}
	int32_t i = 0;
	for (; i < n_new && grid_new[i] < grid[0]; i++) {
		f_new[i] = std::numeric_limits<double>::quiet_NaN();
	}
	int32_t idx = 0;
	for (; i < n_new && grid_new[i] <= grid[n-1]; i++) {
		for (; !(grid[idx] <= grid_new[i] && grid_new[i] <= grid[idx + 1]); idx++);
		f_new[i] = linef(grid[idx],f[idx], grid[idx+1], f[idx+1], grid_new[i]);
	}

	for (; i < n_new; i++) {
		f_new[i] = std::numeric_limits<double>::quiet_NaN();
	}
}