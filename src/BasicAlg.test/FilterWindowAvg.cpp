#include <gtest/gtest.h>
#include <vector>
#include "basic_alg.h"


TEST(FilterWindowAvg, r_window_null) {
	std::vector<double> arr = { 1,2,3,4,5 };
	std::vector<double> arr_filtered (arr.size(),0);
	int32_t r_window = 0;
	FilterWindowAvg((int32_t)arr.size(), arr.data(), r_window, arr_filtered.data());
	for (int32_t i = 0; i < arr.size(); i++) {
		EXPECT_NEAR(arr[i], arr_filtered[i], 1e-10);
	}
}

TEST(FilterWindowAvg, r_window_one) {
	std::vector<double> arr = { 1,2,3,4,5};
	std::vector<double> arr_filtered(arr.size(), 0);
	int32_t r_window = 1;
	FilterWindowAvg((int32_t)arr.size(), arr.data(), r_window, arr_filtered.data());
	EXPECT_NEAR((arr[0]+ arr[1])/2, arr_filtered[0], 1e-10);
	for (int32_t i = 1; i < arr.size()-1; i++) {
		EXPECT_NEAR(arr[i], arr_filtered[i], 1e-10);
	}
	EXPECT_NEAR((arr[arr.size()-1] + arr[arr.size()-2]) / 2, arr_filtered[arr.size() - 1], 1e-10);
}