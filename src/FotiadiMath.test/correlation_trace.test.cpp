#include <gtest/gtest.h>
#include <filesystem>
#include <iostream>
#include "fotiadi_math_private.h"
#include "rapidcsv.h"
#include "save_csv.h"
#include "test_utils.h"

namespace correlation_trace_test
{
	void prepare_for_save_csv_trace(
		const std::vector<double>& exp, const int32_t shift_exp,
		const std::vector<double>& syn, const int32_t shift_syn,
		const std::vector<double>& shift_syn_interval,
		std::vector<double>& uexp, std::vector<double>& usyn, std::vector<double>& ushift_syn
	)
	{
		const auto max_shift = std::max<int32_t>(shift_exp, shift_syn);
		const auto n_interval = shift_syn_interval.size();
		const auto size_tail = std::max<int32_t>(exp.size() - shift_exp - n_interval, syn.size() - shift_syn - n_interval);
		const auto total_size = max_shift + n_interval + size_tail;
		uexp.resize(total_size);
		usyn.resize(total_size);
		ushift_syn.resize(total_size);

		for (int32_t i = 0; i < total_size; i++) {
			const auto idx_exp = i - (max_shift - shift_exp);
			const auto idx_syn = i - (max_shift - shift_syn);
			const auto idx_shift_syn_interval = i - max_shift;

			uexp[i] = 0 <= idx_exp && idx_exp < exp.size() ? exp[idx_exp] : NAN;
			usyn[i] = 0 <= idx_syn && idx_syn < syn.size() ? syn[idx_syn] : NAN;
			ushift_syn[i] = 0 <= idx_shift_syn_interval && idx_shift_syn_interval < shift_syn_interval.size() ? shift_syn_interval[idx_shift_syn_interval] : NAN;
		}
	}

	void WorkflowProcessedTrace(
		const double time_start_interval,
		const double time_finish_interval,
		const double step_data,
		const double max_time_shift_data,
		const int32_t r_idx_convolution,
		const int32_t r_idx_window_filter,
		const std::vector<double>  time_left,	// временные отсчеты для первой трассы
		const std::vector<double>  amp_left,	// амплитуды для первой трассы
		const std::vector<double>  time_right,	// временные отсчеты для второй трассы
		const std::vector<double>  amp_right,	// амплитуды для второй трассы
		const std::string& namefile_out,
		double& tf1_res, 
		double& tf2_res
	) {
		const double min_norm = 1, max_norm = 101;

		//FM_CorrelationByWaveNormalization(amp_syn.size(), amp_syn.data(), min_norm, max_norm);

		const double time_start = time_start_interval, time_finish = time_finish_interval, step = step_data;
		const double max_time_shift = max_time_shift_data;
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
		int32_t r_idx = r_idx_convolution;
		std::vector<int16_t> result(n_interval);
		std::vector<int16_t> x2y(n_interval);
		std::vector<int16_t> y2x(n_interval);

		const DataForMetricConvolutionWithShift data_for_metric{ r_idx,2.0 / 3,0,0 };
		double tf1;
		FM_CorrelationByWaveAlgorithmForTrace(
			n_interval, signal_x.data() + shift_idx_x,
			signal_y.data() + shift_idx_y,
			30, 0, NULL, NULL, NULL, MetricFunctionConvolutionWithShiftDouble, (const void*)&data_for_metric, x2y.data(), y2x.data(), tf1);
		tf1_res = tf1;
		result = x2y;
		std::vector<int16_t> result_inverse(n_interval);
		std::vector<int16_t> x2y_inverse(n_interval);
		std::vector<int16_t> y2x_inverse(n_interval);
		double tf2=tf1;
		/*FM_CorrelationByWaveAlgorithmForTrace(
			n_interval,
			signal_y.data() + shift_idx_y,
			signal_x.data() + shift_idx_x,
			30, 0, NULL, NULL, NULL, MetricFunctionConvolutionWithShift, (const void*)&data_for_metric, x2y_inverse.data(), y2x_inverse.data(), tf2);
		*/
		tf2_res = tf2;

		std::vector<double> shift_syn_interval(result.size(), 0);
		int32_t r_idx_window = r_idx_window_filter;

		std::vector<double> result_double(result.size());
		for (int32_t i = 0; i < result_double.size(); i++) {
			result_double[i] = result[i] > 0 ? result[i] : NAN;
		}
		FM_RecoverFunctionWithFilterWindowAndLineInterpolationByIndex(result.size(), signal_y.data() + shift_idx_y, result.size(), result_double.data(), r_idx_window, shift_syn_interval.data());

		//std::vector<double> uexp, usyn, ushift_syn;
		//prepare_for_save_csv_trace(signal_x, shift_idx_x, signal_y, shift_idx_y, shift_syn_interval, uexp, usyn, ushift_syn);
		const auto count_interval = result.size();
		if(!namefile_out.empty())
		{
			save_csv(namefile_out,
				{
					std::make_tuple("exp", std::vector <double>(signal_x.data() + shift_idx_x,signal_x.data() + shift_idx_x + count_interval)),
					std::make_tuple("syn", std::vector <double>(signal_y.data() + shift_idx_y,signal_y.data() + shift_idx_y + count_interval)),
					std::make_tuple("shift_syn", shift_syn_interval),
				}
			);
		}
	}

	void WorkflowProcessedTraceNewListMaze(
		const double time_start_interval,
		const double time_finish_interval,
		const double step_data,
		const double max_time_shift_data,
		const int32_t r_idx_convolution,
		const int32_t r_idx_window_filter,
		const std::vector<double>  time_left,	// временные отсчеты для первой трассы
		const std::vector<double>  amp_left,	// амплитуды для первой трассы
		const std::vector<double>  time_right,	// временные отсчеты для второй трассы
		const std::vector<double>  amp_right,	// амплитуды для второй трассы
		const std::string& namefile_out,
		double& tf1_res,
		double& tf2_res
	) {
		const double min_norm = 1, max_norm = 101;

		//FM_CorrelationByWaveNormalization(amp_syn.size(), amp_syn.data(), min_norm, max_norm);

		const double time_start = time_start_interval, time_finish = time_finish_interval, step = step_data;
		const double max_time_shift = max_time_shift_data;
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
		int32_t r_idx = r_idx_convolution;
		std::vector<int16_t> result(n_interval);
		std::vector<int16_t> x2y(n_interval);
		std::vector<int16_t> y2x(n_interval);

		const DataForMetricConvolutionWithShift data_for_metric{ r_idx,2.0 / 3,0,0 };
		double tf1;
		FM_CorrelationByWaveAlgorithmForTraceDoubleNewListMaze(
			n_interval, signal_x.data() + shift_idx_x,
			signal_y.data() + shift_idx_y,
			30, 0, NULL, NULL, NULL, MetricFunctionConvolutionWithShiftDouble, (const void*)&data_for_metric, x2y.data(), y2x.data(), tf1);
		tf1_res = tf1;
		result = x2y;
		std::vector<int16_t> result_inverse(n_interval);
		std::vector<int16_t> x2y_inverse(n_interval);
		std::vector<int16_t> y2x_inverse(n_interval);
		double tf2 = tf1;
		tf2_res = tf2;

		std::vector<double> shift_syn_interval(result.size(), 0);
		int32_t r_idx_window = r_idx_window_filter;

		std::vector<double> result_double(result.size());
		for (int32_t i = 0; i < result_double.size(); i++) {
			result_double[i] = result[i] > 0 ? result[i] : NAN;
		}
		FM_RecoverFunctionWithFilterWindowAndLineInterpolationByIndex(result.size(), signal_y.data() + shift_idx_y, result.size(), result_double.data(), r_idx_window, shift_syn_interval.data());

		//std::vector<double> uexp, usyn, ushift_syn;
		//prepare_for_save_csv_trace(signal_x, shift_idx_x, signal_y, shift_idx_y, shift_syn_interval, uexp, usyn, ushift_syn);
		const auto count_interval = result.size();
		if (!namefile_out.empty())
		{
			save_csv(namefile_out,
				{
					std::make_tuple("exp", std::vector <double>(signal_x.data() + shift_idx_x,signal_x.data() + shift_idx_x + count_interval)),
					std::make_tuple("syn", std::vector <double>(signal_y.data() + shift_idx_y,signal_y.data() + shift_idx_y + count_interval)),
					std::make_tuple("shift_syn", shift_syn_interval),
				}
				);
		}
	}

	TEST(Correlation_trace, ex1) {
		std::vector<double> times_left;
		std::vector<double> amps_left;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/ex1/Los-Yar_41_2D_Trace_1500.csv", times_left, amps_left);

		std::vector<double> times_right;
		std::vector<double> amps_right;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/ex1/Los-Yar_41_Syntetic_Full.csv", times_right, amps_right);

		double tf1, tf2;
		WorkflowProcessedTrace(
			1750,
			2000,
			1,
			50,
			10,
			1,
			times_left,
			amps_left,
			times_right,
			amps_right,
			"ex1_trace.out.csv",
			tf1, tf2
		);
	}

	TEST(Correlation_trace, ex1_newListMaze) {
		std::vector<double> times_left;
		std::vector<double> amps_left;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/ex1/Los-Yar_41_2D_Trace_1500.csv", times_left, amps_left);

		std::vector<double> times_right;
		std::vector<double> amps_right;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/ex1/Los-Yar_41_Syntetic_Full.csv", times_right, amps_right);

		double tf1, tf2;
		WorkflowProcessedTraceNewListMaze(
			1750,
			2000,
			1,
			50,
			10,
			1,
			times_left,
			amps_left,
			times_right,
			amps_right,
			"ex1_trace_new_list_maze.out.csv",
			tf1, tf2
		);
	}

	TEST(Correlation_trace, ex3) {
		std::vector<double> times_left;
		std::vector<double> amps_left;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/ex3/S-Ost_7_3D_Trace.csv", times_left, amps_left);

		std::vector<double> times_right;
		std::vector<double> amps_right;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/ex3/Trace_S-Ost_7_synt.csv", times_right, amps_right);

		double tf1, tf2;
		WorkflowProcessedTrace(
			10,//1750,
			2000,
			2,
			50,
			5,
			1,
			times_left,
			amps_left,
			times_right,
			amps_right,
			"ex3_trace.out.csv",
			tf1, tf2
		);
	}

	TEST(Correlation_trace, ex3_newListMaze) {
		std::vector<double> times_left;
		std::vector<double> amps_left;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/ex3/S-Ost_7_3D_Trace.csv", times_left, amps_left);

		std::vector<double> times_right;
		std::vector<double> amps_right;
		GetTimeAndAmpFromCsv("../../FotiadiMath.test/Data/ex3/Trace_S-Ost_7_synt.csv", times_right, amps_right);

		double tf1, tf2;
		WorkflowProcessedTraceNewListMaze(
			10,//1750,
			2000,
			2,
			50,
			5,
			1,
			times_left,
			amps_left,
			times_right,
			amps_right,
			"ex3_trace_new_list_maze.out.csv",
			tf1, tf2
		);
	}

	// тест на 2D_test_autocorr.txt
	TEST(Correlation_trace, DISABLED_seismic2D_ex1) {
		const std::string namefile_exp = "../../FotiadiMath.test/Data/seismic2D/ex1/2D_test_autocorr.txt";
		std::vector<Trace> tracesVector; // массив всех трасс с сопоставлением времени и амплитуды
		LoadSeismic2DAutocorrTxt(namefile_exp, -1, tracesVector);
		int shift_idx = 20;
		std::vector<double> tfs1(tracesVector.size() - 1);
		std::vector<double> tfs2(tracesVector.size() - 1);
		for(int32_t i = 0; i < tracesVector.size()- shift_idx; i++)
		{
			//std::cout << i << std::endl;
			auto size_l = tracesVector[i].values.size();
			std::vector<double> times_left(size_l);
			std::vector<double> amps_left(size_l);
			GetTimeAndAmp(tracesVector[i], times_left, amps_left); // взял первую трассу, вытащил время и амплитуды

			auto size_r = tracesVector[i+ shift_idx].values.size();
			std::vector<double> times_right(size_r);
			std::vector<double> amps_right(size_r);
			GetTimeAndAmp(tracesVector[i+ shift_idx], times_right, amps_right); // взял первую трассу, вытащил время и амплитуды

			double tf1, tf2;
			WorkflowProcessedTrace(
				100,
				2100,
				2,
				50,
				5,
				1,
				times_left,
				amps_left,
				times_right,
				amps_right,
				"",
				tf1, tf2
			);

			tfs1[i] = tf1;
			tfs2[i] = tf2;
		}

		save_csv("ex4_trace.out.csv",
			{
				std::make_tuple("tfs1", tfs1),
				std::make_tuple("tfs2", tfs2)
			}
		);
	}
}
