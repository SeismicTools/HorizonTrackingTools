#include <gtest/gtest.h>
#include "test_utils.h"
#include "fotiadi_math.h"
#include "fotiadi_math_private.h"

namespace correlation_by_wave_algo_tests
{
	// TODO: n.romanov OLD странные данные нам возвращаются от FM_CorrelationByWaveAlgorithmForTrace. Нужно протестировать
	static TEST(FM_CorrelationByWaveAlgorithmForTraceFloat, ex1)
	{
		const double min_norm = 1, max_norm = 101;

		//FM_CorrelationByWaveNormalization(amp_syn.size(), amp_syn.data(), min_norm, max_norm);
		std::vector<double> time_left;
		std::vector<double> amp_left;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/correlation_by_wave_algo_tests/ex1.csv", time_left, amp_left);

		std::vector<double> time_right;
		std::vector<double> amp_right;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/correlation_by_wave_algo_tests/ex1_second.csv", time_right, amp_right);

		const double time_start = 0, time_finish = 8, step = 2;
		const double max_time_shift = 1;
		const int32_t n_array_grid_x = (time_left.back() - time_left.front()) / step + 1;
		std::vector<double> signal_x(n_array_grid_x);
		const int32_t n_array_grid_y = (time_right.back() - time_right.front()) / step + 1;
		std::vector<double> signal_y(n_array_grid_y);
		int32_t n_interval;
		int32_t n_signal_idx_x, shift_idx_x;
		int32_t n_signal_idx_y, shift_idx_y;
		int32_t max_shift_point_idx;
		FM_PrepareDataForCorrelation(
			time_left.size(), time_left.data(), amp_left.data(),
			time_right.size(), time_right.data(), amp_right.data(),
			0, NULL, NULL, NULL, time_start, time_finish, step, max_time_shift,
			n_interval,
			signal_x.size(), n_signal_idx_x, shift_idx_x, signal_x.data(),
			signal_y.size(), n_signal_idx_y, shift_idx_y, signal_y.data(),
			NULL, NULL, NULL, max_shift_point_idx
		);
		int32_t r_idx = 2;
		std::vector<int16_t> result(n_interval);
		std::vector<int16_t> x2y(n_interval);
		std::vector<int16_t> y2x(n_interval);

		const DataForMetricConvolutionWithShift data_for_metric{ r_idx,2.0 / 3,0,0 };
		double tf1;
		FM_CorrelationByWaveAlgorithmForTrace(
			n_interval, signal_x.data() + shift_idx_x,
			signal_y.data() + shift_idx_y,
			1, 0, NULL, NULL, NULL, MetricFunctionConvolutionWithShiftDouble, (const void*)&data_for_metric, x2y.data(), y2x.data(), tf1);
	
		double tf1_res = tf1;
		result = x2y;
		std::vector<int16_t> result_inverse(n_interval);
		std::vector<int16_t> x2y_inverse(n_interval);
		std::vector<int16_t> y2x_inverse(n_interval);
		double tf2 = tf1;
		/*FM_CorrelationByWaveAlgorithmForTrace(
			n_interval,
			signal_y.data() + shift_idx_y,
			signal_x.data() + shift_idx_x,
			30, 0, NULL, NULL, NULL, MetricFunctionConvolutionWithShift, (const void*)&data_for_metric, x2y_inverse.data(), y2x_inverse.data(), tf2);
		*/
		double tf2_res = tf2;

		std::vector<double> shift_syn_interval(result.size(), 0);
		int32_t r_idx_window = 1;

		std::vector<double> result_double(result.size());
		for (int32_t i = 0; i < result_double.size(); i++) {
			result_double[i] = result[i] > 0 ? result[i] : NAN;
		}
		FM_RecoverFunctionWithFilterWindowAndLineInterpolationByIndex(result.size(), signal_y.data() + shift_idx_y, result.size(), result_double.data(), r_idx_window, shift_syn_interval.data());

		//std::vector<double> uexp, usyn, ushift_syn;
		//prepare_for_save_csv_trace(signal_x, shift_idx_x, signal_y, shift_idx_y, shift_syn_interval, uexp, usyn, ushift_syn);
		const auto count_interval = result.size();
	}

	static TEST(FM_CorrelationByWaveAlgorithmForTraceFloat, ex2)
	{
		const double min_norm = 1, max_norm = 101;

		//FM_CorrelationByWaveNormalization(amp_syn.size(), amp_syn.data(), min_norm, max_norm);
		std::vector<double> time_left;
		std::vector<double> amp_left;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/correlation_by_wave_algo_tests/ex2.csv", time_left, amp_left);

		std::vector<double> time_right;
		std::vector<double> amp_right;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/correlation_by_wave_algo_tests/ex2_second.csv", time_right, amp_right);

		const double time_start = 0, time_finish = 8, step = 2;
		const double max_time_shift = 1;
		const int32_t n_array_grid_x = (time_left.back() - time_left.front()) / step + 1;
		std::vector<double> signal_x(n_array_grid_x);
		const int32_t n_array_grid_y = (time_right.back() - time_right.front()) / step + 1;
		std::vector<double> signal_y(n_array_grid_y);
		int32_t n_interval;
		int32_t n_signal_idx_x, shift_idx_x;
		int32_t n_signal_idx_y, shift_idx_y;
		int32_t max_shift_point_idx;
		FM_PrepareDataForCorrelation(
			time_left.size(), time_left.data(), amp_left.data(),
			time_right.size(), time_right.data(), amp_right.data(),
			0, NULL, NULL, NULL, time_start, time_finish, step, max_time_shift,
			n_interval,
			signal_x.size(), n_signal_idx_x, shift_idx_x, signal_x.data(),
			signal_y.size(), n_signal_idx_y, shift_idx_y, signal_y.data(),
			NULL, NULL, NULL, max_shift_point_idx
		);
		int32_t r_idx = 2;
		std::vector<int16_t> result(n_interval);
		std::vector<int16_t> x2y(n_interval);
		std::vector<int16_t> y2x(n_interval);

		const DataForMetricConvolutionWithShift data_for_metric{ r_idx,2.0 / 3,0,0 };
		double tf1;
		FM_CorrelationByWaveAlgorithmForTrace(
			n_interval, signal_x.data() + shift_idx_x,
			signal_y.data() + shift_idx_y,
			1, 0, NULL, NULL, NULL, MetricFunctionConvolutionWithShiftDouble, (const void*)&data_for_metric, x2y.data(), y2x.data(), tf1);

		double tf1_res = tf1;
		result = x2y;
		std::vector<int16_t> result_inverse(n_interval);
		std::vector<int16_t> x2y_inverse(n_interval);
		std::vector<int16_t> y2x_inverse(n_interval);
		double tf2 = tf1;
		/*FM_CorrelationByWaveAlgorithmForTrace(
			n_interval,
			signal_y.data() + shift_idx_y,
			signal_x.data() + shift_idx_x,
			30, 0, NULL, NULL, NULL, MetricFunctionConvolutionWithShift, (const void*)&data_for_metric, x2y_inverse.data(), y2x_inverse.data(), tf2);
		*/
		double tf2_res = tf2;

		std::vector<double> shift_syn_interval(result.size(), 0);
		int32_t r_idx_window = 1;

		std::vector<double> result_double(result.size());
		for (int32_t i = 0; i < result_double.size(); i++) {
			result_double[i] = result[i] > 0 ? result[i] : NAN;
		}
		FM_RecoverFunctionWithFilterWindowAndLineInterpolationByIndex(result.size(), signal_y.data() + shift_idx_y, result.size(), result_double.data(), r_idx_window, shift_syn_interval.data());

		//std::vector<double> uexp, usyn, ushift_syn;
		//prepare_for_save_csv_trace(signal_x, shift_idx_x, signal_y, shift_idx_y, shift_syn_interval, uexp, usyn, ushift_syn);
		const auto count_interval = result.size();
	}

	//static TEST(FM_CorrelationByWaveAlgorithmForTraceFloat, ex4)
	//{
	//	std::vector<double> time_left;
	//	std::vector<double> amp_left;
	//	GetTimeAndAmpFromCsv(
	//		"../../FotiadiMath.test/Data/correlation_by_wave_algo_tests/ex4.csv", 
	//		time_left, 
	//		amp_left
	//	);

	//	std::vector<double> time_right;
	//	std::vector<double> amp_right;
	//	GetTimeAndAmpFromCsv(
	//		"../../FotiadiMath.test/Data/correlation_by_wave_algo_tests/ex4_second.csv", 
	//		time_right, 
	//		amp_right
	//	);

	//	const double time_start = 0, time_finish = 10, step = 1;
	//	const double max_time_shift = 1;

	//	const int32_t n_array_grid_x = (time_left.back() - time_left.front()) / step + 1;
	//	const int32_t n_array_grid_y = (time_right.back() - time_right.front()) / step + 1;
	//	int32_t n_interval;
	//	int32_t n_signal_idx_x, shift_idx_x;
	//	int32_t n_signal_idx_y, shift_idx_y;
	//	int32_t r_idx = 1;
	//	std::vector<int16_t> result(n_interval);
	//	std::vector<int16_t> x2y(n_interval);
	//	std::vector<int16_t> y2x(n_interval);

	//	const DataForMetricConvolutionWithShift data_for_metric{ r_idx,2.0 / 3,0,0 };
	//	double tf1;
	//	FM_CorrelationByWaveAlgorithmForTrace(
	//		n_interval, 
	//		amp_left.data(),
	//		amp_right.data(),
	//		1, 
	//		0, 
	//		NULL, 
	//		NULL, 
	//		NULL, 
	//		MetricFunctionConvolutionWithShiftDouble, 
	//		(const void*)&data_for_metric, 
	//		x2y.data(), 
	//		y2x.data(), 
	//		tf1);

	//	double tf1_res = tf1;
	//	result = x2y;
	//	std::vector<int16_t> result_inverse(n_interval);
	//	std::vector<int16_t> x2y_inverse(n_interval);
	//	std::vector<int16_t> y2x_inverse(n_interval);
	//	double tf2 = tf1;
	//	/*FM_CorrelationByWaveAlgorithmForTrace(
	//		n_interval,
	//		signal_y.data() + shift_idx_y,
	//		signal_x.data() + shift_idx_x,
	//		30, 0, NULL, NULL, NULL, MetricFunctionConvolutionWithShift, (const void*)&data_for_metric, x2y_inverse.data(), y2x_inverse.data(), tf2);
	//	*/
	//	double tf2_res = tf2;

	//	std::vector<double> shift_syn_interval(result.size(), 0);
	//	int32_t r_idx_window = 1;

	//	std::vector<double> result_double(result.size());
	//	for (int32_t i = 0; i < result_double.size(); i++) {
	//		result_double[i] = result[i] > 0 ? result[i] : NAN;
	//	}
	//	FM_RecoverFunctionWithFilterWindowAndLineInterpolationByIndex(result.size(), signal_y.data() + shift_idx_y, result.size(), result_double.data(), r_idx_window, shift_syn_interval.data());

	//	//std::vector<double> uexp, usyn, ushift_syn;
	//	//prepare_for_save_csv_trace(signal_x, shift_idx_x, signal_y, shift_idx_y, shift_syn_interval, uexp, usyn, ushift_syn);
	//	const auto count_interval = result.size();
	//}

	static TEST(FM_CorrelationByWaveAlgorithmForTraceFloat, ex4)
	{
		const double min_norm = 1, max_norm = 101;

		//FM_CorrelationByWaveNormalization(amp_syn.size(), amp_syn.data(), min_norm, max_norm);
		std::vector<double> time_left;
		std::vector<double> amp_left;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/correlation_by_wave_algo_tests/ex1.csv", time_left, amp_left);

		std::vector<double> time_right;
		std::vector<double> amp_right;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/correlation_by_wave_algo_tests/ex1_second.csv", time_right, amp_right);

		const double max_time_shift = 2;

		int32_t r_idx = 1;
		std::vector<int16_t> x2y(amp_left.size());
		std::vector<int16_t> y2x(amp_left.size());

		const DataForMetricConvolutionWithShift data_for_metric{ r_idx, 2.0 / 3, 0, 0 };
		double tf1;
		FM_CorrelationByWaveAlgorithmForTrace(
			amp_left.size(), // n_grid
			amp_left.data(),
			amp_right.data(),
			max_time_shift, // max_shift_point_idx
			0, // n_fix_borders
			NULL, // n_fix_borders
			NULL,
			NULL,
			MetricFunctionSimpleConvolutionDouble,
			(const void*)&data_for_metric,
			x2y.data(),
			y2x.data(),
			tf1);
	}

	static TEST(FM_CorrelationByWaveAlgorithmForTraceFloat, ex5)
	{
		const double min_norm = 1, max_norm = 101;

		//FM_CorrelationByWaveNormalization(amp_syn.size(), amp_syn.data(), min_norm, max_norm);
		std::vector<double> time_left;
		std::vector<double> amp_left;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/correlation_by_wave_algo_tests/ex5.csv", time_left, amp_left);

		std::vector<double> time_right;
		std::vector<double> amp_right;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/correlation_by_wave_algo_tests/ex5_second.csv", time_right, amp_right);

		const double max_time_shift = 2;

		int32_t r_idx = 1;
		std::vector<int16_t> x2y(amp_left.size());
		std::vector<int16_t> y2x(amp_left.size());

		const DataForMetricConvolutionWithShift data_for_metric{ r_idx, 2.0 / 3, 0, 0 };
		double tf1;
		FM_CorrelationByWaveAlgorithmForTrace(
			amp_left.size(), // n_grid
			amp_left.data(),
			amp_right.data(),
			max_time_shift, // max_shift_point_idx
			0, // n_fix_borders
			NULL, // n_fix_borders
			NULL, 
			NULL, 
			MetricFunctionSimpleConvolutionDouble,
			(const void*)&data_for_metric, 
			x2y.data(), 
			y2x.data(), 
			tf1);
	}
}
