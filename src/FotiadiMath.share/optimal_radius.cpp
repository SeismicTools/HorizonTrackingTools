#include <vector>
#include <list>
#include <algorithm>
#include <limits>
#include <cmath>
#include <memory>
#include <numeric>
#include "fotiadi_math.h"

int32_t  FM_DefineOptimalRadius(
	const int32_t n_time,
	const double signals[],
	int32_t& r_idx
) {
	int32_t i;
	//Ищем первое не нулевое значение;
	for (i = 0; i < n_time && std::isnan(signals[i]); i++);
	if (i >= n_time)
		return FM_ERROR_PARAMETRS_NOT_VALID;
	std::list<int32_t> intervals;
	int32_t prev_idx = i;
	for (; i < n_time; i++) {
		if (!std::isnan(signals[i]) && signals[prev_idx] * signals[i] < 0) {
			intervals.push_back(i - prev_idx);
			prev_idx = i;
		}
	}
	intervals.push_back(i - prev_idx);
	std::vector<int32_t> v_intervals(intervals.cbegin(), intervals.cend());
	std::sort(v_intervals.begin(), v_intervals.end());
	//r_idx = (int32_t)(v_intervals[v_intervals.size() / 2] / 2.0 + 0.5);
	r_idx = (int32_t)(v_intervals[v_intervals.size() / 2]);
	return FM_ERROR_NO;
}

int32_t  FM_DefineOptimalRadius2D(
	const int32_t n_trace,
	const int32_t n_time,
	const double signals[],
	int32_t& r_idx
) {
	std::vector<int32_t> idx_r_traces;
#pragma omp parallel for
	for (int64_t i = 0; i < n_trace; i++) {
		int32_t cur_r_idx;
		auto code = FM_DefineOptimalRadius(n_time, signals + (i * n_time), cur_r_idx);
		if (code != FM_ERROR_NO) {
			cur_r_idx = -1;
		}
		idx_r_traces[i] = cur_r_idx;
	}
	std::sort(idx_r_traces.begin(), idx_r_traces.end());
	r_idx = idx_r_traces[idx_r_traces.size() / 2];
	if (r_idx < 0)
		r_idx = 0;
	return FM_ERROR_NO;
}