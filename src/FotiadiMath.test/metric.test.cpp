#include <gtest/gtest.h>
#include "fotiadi_math.h"
#include "rapidcsv.h"

#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

TEST(MetricFunctionSimpleConvolution, Simple) {
	const double etalon = std::sqrt(56/3.0);
	const double f1[] = {0,1,2,3,4,5,6,7,8,9};
	const double f2[] = { 0,1,2,3,4,5,6,7,8,9 };
	auto n_f1 = sizeof(f1) / sizeof(double);
	auto n_f2 = sizeof(f2) / sizeof(double);
	int32_t r_idx = 1;
	auto metric=MetricFunctionSimpleConvolution(n_f1, f1, 3, n_f2, f2, 6, &r_idx);
	EXPECT_NEAR(metric, etalon, 1e-6);
	EXPECT_EQ(1, 1);
}

TEST(MetricFunctionSimpleConvolution, SimpleWithNaN) {
	const double etalon = 1;
	const double f1[] = { NAN,1,2,3,4,5,6,7,8,9 };
	const double f2[] = { NAN,1,2,3,3,4,5,6,7,8,9 };
	auto n_f1 = sizeof(f1) / sizeof(double);
	auto n_f2 = sizeof(f2) / sizeof(double);
	int32_t r_idx = 1;
	auto metric = MetricFunctionSimpleConvolution(n_f1, f1, 0, n_f2, f2, 0, &r_idx);
	EXPECT_NEAR(metric, etalon, 1e-6);
}

TEST(MetricFunctionSimpleConvolution, ex1) {
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
}