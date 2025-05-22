#include <gtest/gtest.h>
#include <filesystem>
#include <iostream>
#include "fotiadi_math_private.h"
#include "test_utils.h" 
TEST(Normalization, simple) {
	double data[] = {0,6,3};
	constexpr size_t n = sizeof(data) / sizeof(double);
	const double min = 1,max= 2;
	FM_CorrelationByWaveNormalization(n,data,min,max);
	double etalon[] = { 1,2,1.5 };
	EXPECT_TRUE(ArraysMatch(data, etalon,1e-12));
}

TEST(Normalization, simple_invert) {
	double data[] = { 0,6,3 };
	constexpr size_t n = sizeof(data) / sizeof(double);
	const double min = 2, max = 1;
	FM_CorrelationByWaveNormalization(n, data, min, max);
	double etalon[] = { 2,1,1.5 };
	EXPECT_TRUE(ArraysMatch(data, etalon, 1e-12));
}

TEST(Normalization, simple_nan) {
	double data[] = { 0,6,NAN};
	constexpr size_t n = sizeof(data) / sizeof(double);
	const double min = 1, max = 2;
	FM_CorrelationByWaveNormalization(n, data, min, max);
	double etalon[] = { 1,2,NAN };
	EXPECT_TRUE(ArraysMatch(data, etalon, 1e-12));
}