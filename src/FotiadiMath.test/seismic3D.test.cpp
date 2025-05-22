#include <gtest/gtest.h>
#include "test_utils.h"
#include "fotiadi_math.h"
#include "save_csv.h"
#include <omp.h>
#include <fstream>

class Seismic3D : public ::testing::Test
{
protected:
	void SetUp()
	{
		size_t numOfTraces = 41;//2;//41;
		int32_t ntrace = -1;//4;//41;
		std::vector<std::pair<int32_t, std::string>> names_files_trace(numOfTraces);
		const int startingTracesAlign = 7110;
		for (int i = 0; i < numOfTraces; i++)
		{
			std::string str = "../../FotiadiMath.test/Data/seismic3D/ex1/inline_";
			str = str + std::to_string(startingTracesAlign + i) + std::string(".txt");
			std::pair<int32_t, std::string> pair(startingTracesAlign + i, str);
			names_files_trace[i] = pair;
		}
		size_t numOfBorders = 2;
		std::vector<std::string> names_files_borders(numOfBorders);
		names_files_borders[0] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_bottom.txt");
		names_files_borders[1] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_rapper.txt");

		/*std::vector<double> time, signals;
		int32_t n, m, num_borders;
		std::vector<int16_t> borders;*/
		LoadSeismic3D(names_files_trace, names_files_borders, time, n, m, signals, num_borders, borders, ntrace);
		border_points_r= std::vector<int16_t>(borders.size(), 1);
	}
	void TearDown()
	{
	}
	std::vector<double> time, signals;
	int32_t n, m, num_borders;
	std::vector<int16_t> borders;
	std::vector<int16_t> border_points_r;


	void WriteSignal(const std::string& name_file) {
		std::vector<std::tuple<std::string, std::vector < double>>> data(n*m);
		for (int32_t i = 0; i < n; i++) {
			for (int32_t j = 0; j < m; j++) {
				const auto name_curve = std::string("T") + "_" + std::to_string(i) + "_" + std::to_string(j);
				auto ptr_start = signals.data() + (i * m + j);
				std::vector<double> data_curve(ptr_start, ptr_start+ time.size());
				data[i * m + j] = std::make_tuple(name_curve, data_curve);
			}
		}
		save_csv(name_file, data);
	}
	void WriteBorders(const std::string& name_file) {
		std::vector<std::tuple<std::string, std::vector < double>>> data(num_borders);
		for (int32_t k = 0; k < num_borders; k++) {
			std::vector<double> data_curve(n * m);
			for (int32_t i = 0; i < n; i++) {
				for (int32_t j = 0; j < m; j++) {
					data_curve[i * m + j] = borders[(i * m + j)* num_borders +k];
				}
			}
			const auto name_curve = std::string("Border_") + std::to_string(k);
			data[k] = std::make_tuple(name_curve, data_curve);
		}
		save_csv(name_file, data);
	}

	void WriteBorders(const std::string& name_file,int32_t num_border) {
		std::ofstream out(name_file);
		std::vector<double> data_curve(n * m);
		for (int32_t i = 0; i < n; i++) {
			for (int32_t j = 0; j < m; j++) {
				out<< borders[(i * m + j) * num_borders + num_border] << "\t";
			}
			out << std::endl;
		}
		out.close();
	}
};

int32_t CallBackSeismic3D(const double progress) {
	auto num = omp_get_thread_num();
	std::cout << progress << std::endl;
	/*if (progress > 0.75) {
		return FM_BREAK_CALCULATE;
	}*/
	return FM_CONTINUE_CALCULATE;
}

TEST_F(Seismic3D, trace_borders_Debug) {
	WriteSignal("signals.csv");
	WriteBorders("borders.csv");
	WriteBorders("borders_0.csv",0);
	int32_t max_shift_point_idx = 10;
	int32_t r_idx = 40;
	FM_Seismic3DTraceBorders(
		n,
		m,
		time.size(),
		signals.data(),
		max_shift_point_idx,
		num_borders,
		borders.data(),
		CallBackSeismic3D,
		r_idx);
	WriteBorders("borders_fix.csv");
	WriteBorders("borders_fix_0.csv",0);
}


// либо слишком долгий тест, либо что-то не работает
TEST_F(Seismic3D, DISABLED_Debug) {

	auto num = omp_get_thread_num();
	int32_t max_shift_point_idx = 15;
	void* p_obj = nullptr;
	FM_Seismic3DCorrelationByWaveCreateObject(
		n,			//!< количество трасс по оси X
		m,			//!< количество трасс по оси Y
		time.size(),				//!< количество временных отсчётов
		signals.data(),			    //!< сигналы [n_trace_y,n_trace_x,n_time]
		max_shift_point_idx,	//!< Максимальный сдвиг в каждой точки по индексам между соседними трассами
		num_borders,         //!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
		borders.data(),		//!< -1 -> значит нет фиксации точки [n_trace_y,n_trace_x, n_fix_curves]
		border_points_r.data(),    //!< для каждой точки задается окрестность в которой точку можно сдвинуть [n_trace_y,n_trace_x,n_fix_curves]
		CallBackSeismic3D,			//!< обратная связь
		&p_obj				//!< [out] Коды прослеженных пластов  [n_trace,n_time]
	);

	FM_Seismic3DCorrelationByWaveDeleteObject(p_obj);
	//EXPECT_EQ(n, numOfTraces);
}