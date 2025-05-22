#include <gtest/gtest.h>
#include <rapidcsv.h>
#include "test_utils.h"
#include <fotiadi_math.h>

namespace define_optimal_radius_tests
{
	TEST(DefineOptimalRadius, debug)
	{
		std::vector<double> amp_ex = { NAN,1,-2,-3,NAN,-4,2,4,NAN,5,6,-7,NAN };
		int32_t optimal_r;
		auto res = FM_DefineOptimalRadius(amp_ex.size(), amp_ex.data(), optimal_r);
		EXPECT_EQ(res, 0);
	}
	TEST(DefineOptimalRadius, ex1_expr)
	{
		const std::string namefile_exp = "../../FotiadiMath.test/Data/ex1/Los-Yar_41_2D_Trace_1500.csv";

		rapidcsv::Document doc_ex(
			namefile_exp,
			rapidcsv::LabelParams(),
			rapidcsv::SeparatorParams(';')
		);

		std::vector<double> amp_ex = doc_ex.GetColumn<double>("amp");
		fix_zero(amp_ex);

		int32_t optimal_r;
		auto res = FM_DefineOptimalRadius(amp_ex.size(), amp_ex.data(), optimal_r);
		EXPECT_EQ(res, 0);
	}

	TEST(DefineOptimalRadius, ex1_synt)
	{
		const std::string namefile_exp = "../../FotiadiMath.test/Data/ex1/Los-Yar_41_Syntetic_Full.csv";

		rapidcsv::Document doc_ex(
			namefile_exp,
			rapidcsv::LabelParams(),
			rapidcsv::SeparatorParams(';')
		);

		std::vector<double> amp_ex = doc_ex.GetColumn<double>("amp");
		fix_zero(amp_ex);

		int32_t optimal_r;
		auto res = FM_DefineOptimalRadius(amp_ex.size(), amp_ex.data(), optimal_r);
		EXPECT_EQ(res, 0);
	}

	TEST(DefineOptimalRadius, ex2_expr)
	{
		const std::string namefile_exp = "../../FotiadiMath.test/Data/ex2/Ost_417_3D_Trace.csv";

		rapidcsv::Document doc_ex(
			namefile_exp,
			rapidcsv::LabelParams(),
			rapidcsv::SeparatorParams(';')
		);

		std::vector<double> amp_ex = doc_ex.GetColumn<double>("amp");
		fix_zero(amp_ex);

		int32_t optimal_r;
		auto res = FM_DefineOptimalRadius(amp_ex.size(), amp_ex.data(), optimal_r);
		EXPECT_EQ(res, 0);
	}

	TEST(DefineOptimalRadius, ex2_synt)
	{
		const std::string namefile_exp = "../../FotiadiMath.test/Data/ex2/Ost_417_Syntetic.csv";

		rapidcsv::Document doc_ex(
			namefile_exp,
			rapidcsv::LabelParams(),
			rapidcsv::SeparatorParams(';')
		);

		std::vector<double> amp_ex = doc_ex.GetColumn<double>("amp");
		fix_zero(amp_ex);

		int32_t optimal_r;
		auto res = FM_DefineOptimalRadius(amp_ex.size(), amp_ex.data(), optimal_r);
		EXPECT_EQ(res, 0);
	}

	TEST(DefineOptimalRadius, ex3_expr)
	{
		const std::string namefile_exp = "../../FotiadiMath.test/Data/ex3/S-Ost_7_3D_Trace.csv";

		rapidcsv::Document doc_ex(
			namefile_exp,
			rapidcsv::LabelParams(),
			rapidcsv::SeparatorParams(';')
		);

		std::vector<double> amp_ex = doc_ex.GetColumn<double>("amp");
		fix_zero(amp_ex);

		int32_t optimal_r;
		auto res = FM_DefineOptimalRadius(amp_ex.size(), amp_ex.data(), optimal_r);
		EXPECT_EQ(res, 0);
	}

	TEST(DefineOptimalRadius, ex3_synt)
	{
		const std::string namefile_exp = "../../FotiadiMath.test/Data/ex3/Trace_S-Ost_7_synt.csv";

		rapidcsv::Document doc_ex(
			namefile_exp,
			rapidcsv::LabelParams(),
			rapidcsv::SeparatorParams(';')
		);

		std::vector<double> amp_ex = doc_ex.GetColumn<double>("amp");
		fix_zero(amp_ex);

		int32_t optimal_r;
		auto res = FM_DefineOptimalRadius(amp_ex.size(), amp_ex.data(), optimal_r);
		EXPECT_EQ(res, 0);
	}

	TEST(DefineOptimalRadius, ex1_2d)
	{
		const std::string namefile_exp = "../../FotiadiMath.test/Data/seismic2D/ex1/2D_test_autocorr.txt";
		std::vector<Trace> tracesVector; // массив всех трасс с сопоставлением времени и амплитуды
		LoadSeismic2DAutocorrTxt(namefile_exp, -1, tracesVector);

		std::vector<int32_t> radiuses(tracesVector.size()); // здесь будут лежать все посчитанные радиусы для каждой трассы
		for (int i = 0; i < tracesVector.size(); i++)
		{
			size_t size = tracesVector[i].values.size();
			std::vector<double> time_ex(size);
			std::vector<double> amp_ex(size);
			GetTimeAndAmp(tracesVector[i], time_ex, amp_ex);
			fix_zero(amp_ex);

			int32_t optimal_r;
			auto res = FM_DefineOptimalRadius(amp_ex.size(), amp_ex.data(), optimal_r);
			EXPECT_EQ(res, 0);
			radiuses[i] = optimal_r;
		}

		int a = 5 + 1;
	}
}
