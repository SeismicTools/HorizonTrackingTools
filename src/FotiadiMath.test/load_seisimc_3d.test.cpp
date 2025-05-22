#include <gtest/gtest.h>
#include "test_utils.h"
#include <save_csv.h>
#include "rapidcsv.h"
#include <chrono>

namespace load_seisimc_3d_test
{
	TEST(BasicTest, Ex1) {
		size_t numOfTraces = 41;
		std::vector<std::pair<int32_t, std::string>> names_files_trace(numOfTraces);
		const int startingTracesAlign = 7110;
		for(int i = 0; i < numOfTraces; i++)
		{
			std::string str = "../../FotiadiMath.test/Data/seismic3D/ex1/inline_";
			str = str + std::to_string(startingTracesAlign + i) + std::string(".txt");
			std::pair<int32_t, std::string> pair(startingTracesAlign, str);
			names_files_trace[i] = pair;
		}
		size_t numOfBorders = 3;
		std::vector<std::string> names_files_borders(numOfBorders);
		names_files_borders[0] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_bottom.txt");
		names_files_borders[1] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_rapper.txt");
		names_files_borders[2] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_top.txt");

		std::vector<double> time, signals;
		int32_t n, m, num_borders;
		std::vector<int16_t> borders;
		LoadSeismic3D(names_files_trace, names_files_borders, time, n, m, signals, num_borders, borders);

		EXPECT_EQ(n, numOfTraces);
	}

	TEST(TemplateTest, DoubleTest) {
		size_t numOfTraces = 1;
		std::vector<std::pair<int32_t, std::string>> names_files_trace(numOfTraces);
		const int startingTracesAlign = 7110;
		for (int i = 0; i < numOfTraces; i++)
		{
			std::string str = "../../FotiadiMath.test/Data/seismic3D/ex1/inline_";
			str = str + std::to_string(startingTracesAlign + i) + std::string(".txt");
			std::pair<int32_t, std::string> pair(startingTracesAlign, str);
			names_files_trace[i] = pair;
		}
		size_t numOfBorders = 2;
		std::vector<std::string> names_files_borders(numOfBorders);
		names_files_borders[0] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_bottom.txt");
		names_files_borders[1] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_rapper.txt");

		std::vector<double> time, signals;
		int32_t n, m, num_borders;
		std::vector<int16_t> borders;
		LoadSeismic3DDouble(names_files_trace, names_files_borders, time, n, m, signals, num_borders, borders);

		EXPECT_EQ(n, numOfTraces);
	}

	TEST(TemplateTest, FloatTest) {
		size_t numOfTraces = 1;
		std::vector<std::pair<int32_t, std::string>> names_files_trace(numOfTraces);
		const int startingTracesAlign = 7110;
		for (int i = 0; i < numOfTraces; i++)
		{
			std::string str = "../../FotiadiMath.test/Data/seismic3D/ex1/inline_";
			str = str + std::to_string(startingTracesAlign + i) + std::string(".txt");
			std::pair<int32_t, std::string> pair(startingTracesAlign, str);
			names_files_trace[i] = pair;
		}
		size_t numOfBorders = 2;
		std::vector<std::string> names_files_borders(numOfBorders);
		names_files_borders[0] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_bottom.txt");
		names_files_borders[1] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_rapper.txt");

		std::vector<float> time, signals;
		int32_t n, m, num_borders;
		std::vector<int16_t> borders;
		LoadSeismic3DFloat(names_files_trace, names_files_borders, time, n, m, signals, num_borders, borders);

		EXPECT_EQ(n, numOfTraces);
	}

	TEST(BasicTest, Ex2) {
		size_t numOfTraces = 1;
		std::vector<std::pair<int32_t, std::string>> names_files_trace(numOfTraces);
		const int startingTracesAlign = 7110;
		for (int i = 0; i < numOfTraces; i++)
		{
			std::string str = "../../FotiadiMath.test/Data/seismic3D/ex1/inline_";
			str = str + std::to_string(startingTracesAlign + i) + std::string(".txt");
			std::pair<int32_t, std::string> pair(startingTracesAlign, str);
			names_files_trace[i] = pair;
		}
		size_t numOfBorders = 2;
		std::vector<std::string> names_files_borders(numOfBorders);
		names_files_borders[0] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_bottom.txt");
		names_files_borders[1] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_rapper.txt");

		std::vector<double> time, signals;
		int32_t n, m, num_borders;
		std::vector<int16_t> borders;
		LoadSeismic3D(names_files_trace, names_files_borders, time, n, m, signals, num_borders, borders);

		EXPECT_EQ(n, numOfTraces);
	}

	TEST(SpecificTest, TwoTracesAndNoBorders) {
		size_t numOfTraces = 2;
		std::vector<std::pair<int32_t, std::string>> names_files_trace(numOfTraces);
		const int startingTracesAlign = 7110;
		for (int i = 0; i < numOfTraces; i++)
		{
			std::string str = "../../FotiadiMath.test/Data/seismic3D/ex1/inline_";
			str = str + std::to_string(startingTracesAlign + i) + std::string(".txt");
			std::pair<int32_t, std::string> pair(startingTracesAlign + i, str);
			names_files_trace[i] = pair;
		}
		size_t numOfBorders = 0;
		std::vector<std::string> names_files_borders(numOfBorders);

		std::vector<double> time, signals;
		// n - число файлов (inline)
		// m - число трасс в файле
		// time - число измерений в одной трассе
		int32_t n, m, num_borders;
		std::vector<int16_t> borders;
		LoadSeismic3D(names_files_trace, names_files_borders, time, n, m, signals, num_borders, borders);

		std::vector<double> ids;
		for(int i = 0; i < n; i++) // обход inline`ов
		{
			for(int j = 0; j < m; j++) // обход трасс
			{
				std::vector<double> values;
				for(int k = 0; k < time.size(); k++) // обход измерений
				{
					if (i == 0 && j == 0)
					{
						ids.push_back(time[k]);
					}
					values.push_back(signals[n*i + m*j + k]);
				}

				save_csv("../../FotiadiMath.test/Data/seismic3D/ex1 csv/inline_" + std::to_string(i) + "_trace_" + std::to_string(j) + ".csv",
					{
						std::make_tuple("id", ids),
						std::make_tuple("value", values),
					}
				);
			}
		}

		EXPECT_EQ(n, numOfTraces);
	}

	TEST(SpecificTest, AllTracesAndNoBorders) {
		size_t numOfTraces = 1;
		std::vector<std::pair<int32_t, std::string>> names_files_trace(numOfTraces);
		const int startingTracesAlign = 7110;
		for (int i = 0; i < numOfTraces; i++)
		{
			std::string str = "../../FotiadiMath.test/Data/seismic3D/ex1/inline_";
			str = str + std::to_string(startingTracesAlign + i) + std::string(".txt");
			std::pair<int32_t, std::string> pair(startingTracesAlign + i, str);
			names_files_trace[i] = pair;
		}
		size_t numOfBorders = 0;
		std::vector<std::string> names_files_borders(numOfBorders);

		std::vector<double> time, signals;
		// n - число файлов (inline)
		// m - число трасс в файле
		// time - число измерений в одной трассе
		int32_t n, m, num_borders;
		std::vector<int16_t> borders;

		auto start = std::chrono::high_resolution_clock::now();
		LoadSeismic3D(names_files_trace, names_files_borders, time, n, m, signals, num_borders, borders);
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = end - start;
		// 9.6 секунд для одного inline`а

		std::vector<double> ids;
		for (int i = 0; i < n; i++) // обход inline`ов
		{
			for (int j = 0; j < m; j++) // обход трасс
			{
				std::vector<double> values;
				for (int k = 0; k < time.size(); k++) // обход измерений
				{
					if (i == 0 && j == 0)
					{
						ids.push_back(time[k]);
					}
					values.push_back(signals[n * i + m * j + k]);
				}

				save_csv("../../FotiadiMath.test/Data/seismic3D/ex1 csv/inline_" + std::to_string(i) + "_trace_" + std::to_string(j) + ".csv",
					{
						std::make_tuple("id", ids),
						std::make_tuple("value", values),
					}
					);
			}
		}

		EXPECT_EQ(n, numOfTraces);
	}

	/// <summary>
	/// Тест, показывающий, что читать из csv, это медленно
	/// </summary>
	/// <param name=""></param>
	/// <param name=""></param>
	TEST(SpecificTest, AllTracesAndNoBordersCsv) {
		size_t numOfInline = 1;
		const int startingTracesAlign = 0;
		const int numOfTraces = 231;
		const int numOfMeasurement = 1001;
		std::vector<int> ids;

		std::vector<double> signals;
		auto start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < numOfInline; i++)
		{
			std::string str = "../../FotiadiMath.test/Data/seismic3D/ex1 csv/inline_";
			str = str + std::to_string(startingTracesAlign + i);// + std::string(".txt");
			std::vector<std::vector<double>> arrays(numOfTraces, std::vector<double>(numOfMeasurement));
			#pragma omp parallel for
			for(int j = 0; j < numOfTraces; j++)
			{
				auto fileToParse = str + std::string("_trace_") + std::to_string(j) + std::string(".csv");
				rapidcsv::Document doc_ex(
					fileToParse,
					rapidcsv::LabelParams(),
					rapidcsv::SeparatorParams(';')
				);
				if(ids.empty())
				{
					auto time_ex = doc_ex.GetColumn<int>("id");
					ids.insert(ids.end(), time_ex.begin(), time_ex.end());
				}
				
				auto amp_ex = doc_ex.GetColumn<double>("value");
				for (size_t k = 0; k < amp_ex.size(); ++k) {
					arrays[j][k] = amp_ex[k];
				}

				//signals.insert(signals.end(), amp_ex.begin(), amp_ex.end());
			}
			for (int j = 0; j < numOfTraces; j++) {
				signals.insert(signals.end(), arrays[j].begin(), arrays[j].end());
			}
		}
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = end - start;
		// 65 секунд

		EXPECT_EQ(1, 1);
	}

	TEST(SaveInlinesToCsv, Save)
	{
		size_t numOfTrace = 0;
		std::pair<int32_t, std::string> name_trace_file;
		const int startingTracesAlign = 7110;

		std::string str = "../../FotiadiMath.test/Data/seismic3D/ex1/inline_";
		str = str + std::to_string(startingTracesAlign + numOfTrace) + std::string(".txt");
		name_trace_file = std::pair<int32_t, std::string>(startingTracesAlign + numOfTrace, str);

		std::vector<double> time, signals;
		int32_t n = 1, m;

		LoadSeismic3DSingleTrace(name_trace_file, time, m, signals);

		std::vector<double> ids;
		for (int i = 0; i < n; i++) // обход inline`ов
		{
			for (int j = 0; j < m; j++) // обход трасс
			{
				std::vector<double> values;
				for (int k = 0; k < time.size(); k++) // обход измерений
				{
					if (i == 0 && j == 0)
					{
						ids.push_back(time[k]);
					}
					values.push_back(signals[n * m * i + m * j + k]);
				}

				save_csv("../../FotiadiMath.test/Data/seismic3D/ex1 csv/inline_" + std::to_string(i) + "_trace_" + std::to_string(j) + ".csv",
					{
						std::make_tuple("id", ids),
						std::make_tuple("value", values),
					}
					);
			}
		}
	}

	// C:\Users\nikit\Documents\GitHub\FotiadiMath\src\x64\Release - сюда сохранял

	// загрузка границ без трасс, НЕВОЗМОЖНА, т.к. границы напрямую привязаны к трассам
	//TEST(SpecificTest, NoTracesAndOnlyBorders) {
	//	size_t numOfTraces = 0;
	//	std::vector<std::pair<int32_t, std::string>> names_files_trace(numOfTraces);

	//	size_t numOfBorders = 3;
	//	std::vector<std::string> names_files_borders(numOfBorders);
	//	names_files_borders[0] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_bottom.txt");
	//	names_files_borders[1] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_rapper.txt");
	//	names_files_borders[2] = std::string("../../FotiadiMath.test/Data/seismic3D/ex1/3d_top.txt");

	//	std::vector<double> time, signals;
	//	int32_t n, m, num_borders;
	//	std::vector<int16_t> borders;
	//	LoadSeismic3D(names_files_trace, names_files_borders, time, n, m, signals, num_borders, borders);

	//	EXPECT_EQ(n, numOfTraces);
	//		}
}
