#include <gtest/gtest.h>
#include "fotiadi_math.h"
#include "rapidcsv.h"
#include "save_csv.h"
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;
#include "fotiadi_math_private.h"
#include "test_utils.h"
//#include "correlation.cpp"

namespace correlation_test
{
	void prepare_for_save_csv(
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

	void WorkflowProcessed(
		const std::string& namefile_exp,
		const std::string& namefile_syn,
		const double time_start_interval,
		const double time_finish_interval,
		const double step_data,
		const double max_time_shift_data,
		const int32_t r_idx_convolution,
		const int32_t r_idx_window_filter,
		const std::string& namefile_out
	) {
		const double min_norm = 1, max_norm = 101;
		rapidcsv::Document doc_ex(
			namefile_exp,
			rapidcsv::LabelParams(),
			rapidcsv::SeparatorParams(';')
		);

		std::vector<double> time_ex = doc_ex.GetColumn<double>("time");
		std::vector<double> amp_ex = doc_ex.GetColumn<double>("amp");
		fix_zero(amp_ex);
		//FM_CorrelationByWaveNormalization(amp_ex.size(), amp_ex.data(), min_norm, max_norm);


		rapidcsv::Document doc_syn(
			namefile_syn,
			rapidcsv::LabelParams(),
			rapidcsv::SeparatorParams(';')
		);

		std::vector<double> time_syn = doc_syn.GetColumn<double>("time");
		std::vector<double> amp_syn = doc_syn.GetColumn<double>("amp");
		fix_zero(amp_syn);
		//FM_CorrelationByWaveNormalization(amp_syn.size(), amp_syn.data(), min_norm, max_norm);

		save_csv("init_" + namefile_out,
			{
				std::make_tuple("exp", amp_ex),
				std::make_tuple("syn", amp_syn),
			}
		);

		const double time_start = time_start_interval, time_finish = time_finish_interval, step = step_data;
		const double max_time_shift = max_time_shift_data;
		const int32_t n_array_grid_x = (time_ex.back() - time_ex.front()) / step + 1;
		std::vector<double> signal_x(n_array_grid_x);
		const int32_t n_array_grid_y = (time_syn.back() - time_syn.front()) / step + 1;
		std::vector<double> signal_y(n_array_grid_y);
		int32_t n_interval;
		int32_t n_signal_idx_x, shift_idx_x;
		int32_t n_signal_idx_y, shift_idx_y;
		int32_t max_shift_point_idx;
		FM_PrepareDataForCorrelation(
			time_ex.size(), time_ex.data(), amp_ex.data(),
			time_syn.size(), time_syn.data(), amp_syn.data(),
			0, NULL, NULL, NULL, time_start, time_finish, step, max_time_shift,
			n_interval,
			signal_x.size(), n_signal_idx_x, shift_idx_x, signal_x.data(),
			signal_y.size(), n_signal_idx_y, shift_idx_y, signal_y.data(),
			NULL, NULL, NULL, max_shift_point_idx
		);
		int32_t r_idx = r_idx_convolution;
		std::vector<double> result(n_interval);
		double tf;
		FM_CorrelationByWaveAlgorithm(
			n_signal_idx_x, shift_idx_x, signal_x.data(),
			n_signal_idx_y, shift_idx_y, signal_y.data(),
			n_interval, 30, 0, NULL, NULL, NULL, MetricFunctionSimpleConvolution, &r_idx, result.data(), tf);
		std::vector<double> shift_syn_interval(result.size(), 0);
		int32_t r_idx_window = r_idx_window_filter;
		FM_RecoverFunctionWithFilterWindowAndLineInterpolationByIndex(n_signal_idx_y, signal_y.data(), result.size(), result.data(), r_idx_window, shift_syn_interval.data());

		std::vector<double> uexp, usyn, ushift_syn;
		prepare_for_save_csv(signal_x, shift_idx_x, signal_y, shift_idx_y, shift_syn_interval, uexp, usyn, ushift_syn);
		save_csv(namefile_out,
			{
				std::make_tuple("exp", uexp),
				std::make_tuple("syn", usyn),
				std::make_tuple("shift_syn", ushift_syn),
			}
		);
	}


	TEST(CurrentPath, Example) {
		auto cp = fs::current_path();
		std::cout << "Current path is " << fs::current_path() << '\n'; // (1)
		//fs::current_path(fs::temp_directory_path()); // (3)
		//cp = fs::current_path();
		std::cout << "Current path is " << fs::current_path() << '\n';
	}

	TEST(Correlation, ex1) {
		WorkflowProcessed(
			"../../FotiadiMath.test/Data/ex1/Los-Yar_41_2D_Trace_1500.csv",
			"../../FotiadiMath.test/Data/ex1/Los-Yar_41_Syntetic_Full.csv",
			1750,
			2000,
			1,
			50,
			10,
			3,
			"ex1.out.csv"
		);
	}

	TEST(Correlation, ex2) {
		WorkflowProcessed(
			"../../FotiadiMath.test/Data/ex2/Ost_417_3D_Trace.csv",
			"../../FotiadiMath.test/Data/ex2/Ost_417_Syntetic.csv",
			1750,
			2000,
			2,
			50,
			5,
			3,
			"ex2.out.csv"
		);
	}

	TEST(Correlation, ex3) {
		WorkflowProcessed(
			"../../FotiadiMath.test/Data/ex3/S-Ost_7_3D_Trace.csv",
			"../../FotiadiMath.test/Data/ex3/Trace_S-Ost_7_synt.csv",
			10,//1750,
			2000,
			2,
			50,
			5,
			3,
			"ex3.out.csv"
		);
	}

	TEST(Correlation, ex4) {
		WorkflowProcessed(
			"../../FotiadiMath.test/Data/ex4/ex4.csv",
			"../../FotiadiMath.test/Data/ex4/ex4_second.csv",
			0, // time_start_interval,
			10, // time_finish_interval
			1, // step_data
			1, // max_time_shift_data
			1, // r_idx_convolution
			3, // r_idx_window_filter
			"../../FotiadiMath.test/Data/ex4/ex4.out.csv"
		);
	}

	TEST(Correlation, DISABLED_ex1_init) {
		rapidcsv::Document doc_ex(
			"../../FotiadiMath.test/Data/ex1/Los-Yar_41_2D_Trace_1500.csv",
			rapidcsv::LabelParams(),
			rapidcsv::SeparatorParams(';')
		);
		std::vector<double> time_ex = doc_ex.GetColumn<double>("time");
		std::vector<double> amp_ex = doc_ex.GetColumn<double>("amp");

		rapidcsv::Document doc_syn(
			"../../FotiadiMath.test/Data/ex1/Los-Yar_41_Syntetic_Full.csv",
			rapidcsv::LabelParams(),
			rapidcsv::SeparatorParams(';')
		);
		std::vector<double> time_syn = doc_syn.GetColumn<double>("time");
		std::vector<double> amp_syn = doc_syn.GetColumn<double>("amp");
		//std::cout << "Read " << col.size() << " values." << std::endl;
		int32_t shift_idx_y = 0;
		for (int i = 0; i < time_syn.size(); i++) {
			if (time_ex[127] <= time_syn[i]) {
				shift_idx_y = i;
				break;
			}
		}

		int32_t r_idx = 10;
		//127-178
		int32_t shift_x = 127 * 2;
		int32_t n_interval = (178 - 127 + 1) * 2;
		std::list<double> amp_ex_fix;
		for (int i = 0; i < amp_ex.size() - 1; i++) {
			amp_ex_fix.push_back(amp_ex[i]);
			amp_ex_fix.push_back((amp_ex[i] + amp_ex[i + 1]) / 2);
		}
		std::vector<double> amp_ex_fix_vector(amp_ex_fix.begin(), amp_ex_fix.end());
		std::vector<double> result(n_interval);
		double tf;
		FM_CorrelationByWaveAlgorithm(amp_ex_fix_vector.size(), shift_x, amp_ex_fix_vector.data(), amp_syn.size(), shift_idx_y, amp_syn.data(), n_interval, 30, 0, NULL, NULL, NULL, MetricFunctionSimpleConvolution, &r_idx, result.data(), tf);
		std::vector<double> amp_syn_shifted(amp_ex_fix_vector.size(), 0);
		std::vector<double> f_recovery(result.size(), 0);
		int32_t r_idx_window = 2;
		FM_RecoverFunctionWithFilterWindowAndLineInterpolationByIndex(amp_syn.size(), amp_syn.data(), result.size(), result.data(), r_idx_window, f_recovery.data());
		for (int i = 0; i < f_recovery.size(); i++) {
			amp_syn_shifted[shift_x + i] = f_recovery[i];
		}
		EXPECT_EQ(1, 1);
		EXPECT_TRUE(true);
	}

	TEST(Correlation, ex2_fix_point) {
		rapidcsv::Document doc_ex(
			"../../FotiadiMath.test/Data/ex2/Ost_417_3D_Trace.csv",
			rapidcsv::LabelParams(),
			rapidcsv::SeparatorParams(';')
		);
		std::vector<double> time_ex = doc_ex.GetColumn<double>("time");
		std::vector<double> amp_ex = doc_ex.GetColumn<double>("amp");

		rapidcsv::Document doc_syn(
			"../../FotiadiMath.test/Data/ex2/Ost_417_Syntetic.csv",
			rapidcsv::LabelParams(),
			rapidcsv::SeparatorParams(';')
		);
		std::vector<double> time_syn = doc_syn.GetColumn<double>("time");
		std::vector<double> amp_syn = doc_syn.GetColumn<double>("amp");

		const double time_start = 1750, time_finish = 2000, step = 2;
		const double max_time_shift = 50;
		const int32_t n_array_grid_x = (time_ex.back() - time_ex.front()) / step + 1;
		std::vector<double> signal_x(n_array_grid_x);
		const int32_t n_array_grid_y = (time_syn.back() - time_syn.front()) / step + 1;
		std::vector<double> signal_y(n_array_grid_y);
		int32_t n_interval;
		int32_t n_signal_idx_x, shift_idx_x;
		int32_t n_signal_idx_y, shift_idx_y;
		int32_t max_shift_point_idx;
		const int32_t n_fix_point_init = 1;
		const double fix_point_init_x[] = { 1800 };
		const double fix_point_init_y[] = { 1800 - 10 };
		const double fix_point_init_r[] = { 1 };
		int32_t fix_point_idx_x[n_fix_point_init];
		int32_t fix_point_idx_y[n_fix_point_init];
		int32_t fix_point_idx_r[n_fix_point_init];

		//double 
		FM_PrepareDataForCorrelation(
			time_ex.size(), time_ex.data(), amp_ex.data(),
			time_syn.size(), time_syn.data(), amp_syn.data(),
			n_fix_point_init, fix_point_init_x, fix_point_init_y, fix_point_init_r, time_start, time_finish, step, max_time_shift,
			n_interval,
			signal_x.size(), n_signal_idx_x, shift_idx_x, signal_x.data(),
			signal_y.size(), n_signal_idx_y, shift_idx_y, signal_y.data(),
			fix_point_idx_x, fix_point_idx_y, fix_point_idx_r, max_shift_point_idx
		);
		int32_t r_idx = 5;
		std::vector<double> result(n_interval);
		double tf;
		FM_CorrelationByWaveAlgorithm(
			n_signal_idx_x, shift_idx_x, signal_x.data(),
			n_signal_idx_y, shift_idx_y, signal_y.data(),
			n_interval, 30, n_fix_point_init, fix_point_idx_x, fix_point_idx_y, fix_point_idx_r,
			MetricFunctionSimpleConvolution, &r_idx, result.data(), tf);
		std::vector<double> shift_syn_interval(result.size(), 0);
		int32_t r_idx_window = 2;
		FM_RecoverFunctionWithFilterWindowAndLineInterpolationByIndex(n_signal_idx_y, signal_y.data(), result.size(), result.data(), r_idx_window, shift_syn_interval.data());

		std::vector<double> uexp, usyn, ushift_syn;
		prepare_for_save_csv(signal_x, shift_idx_x, signal_y, shift_idx_y, shift_syn_interval, uexp, usyn, ushift_syn);
		save_csv("ex2.out.csv",
			{
				std::make_tuple("exp", uexp),
				std::make_tuple("syn", usyn),
				std::make_tuple("shift_syn", ushift_syn),
			}
		);
	}

	// Полный путь к папке с тестовыми данными: C:\Users\nikit\source\repos\FotiadiMath\src\FotiadiMath.test\Data

	// тест на реальных данных из десмана
	TEST(Correlation, couple_of_traces_read_data)
	{
		std::ifstream file("../../FotiadiMath.test/Data/ex5/ex5.txt"); // Открываем файл
		if (!file.is_open()) {
			ASSERT_TRUE(0);
		}

		int n_grid_x;
		file >> n_grid_x; // Читаем размер массива

		std::vector<double> signal_x(n_grid_x); // Создаем массив указанного размера

		for (int i = 0; i < n_grid_x; ++i) {
			file >> signal_x[i]; // Читаем значения в массив
		}

		int n_grid_y;
		file >> n_grid_y;
		std::vector<double> signal_y(n_grid_y);
		for (int i = 0; i < n_grid_y; ++i) {
			file >> signal_y[i];
		}

		// Закрываем файл
		file.close();

		DataForMetricConvolutionWithShift data_for_metric{ (uint16_t)10, 2.0 / 3, 0, 0 };

		auto cur_fm = MetricFunctionSimpleConvolutionDouble(n_grid_x, 
			signal_x.data(),
			0,
			n_grid_y,
			signal_y.data(),
			0,
			(const void*)&data_for_metric
		);

		ASSERT_TRUE(1);
	}
}
