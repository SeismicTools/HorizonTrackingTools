#include <gtest/gtest.h>
#include <vector>
#include "std_alg.h"


TEST(binary_search, r_window_eq) {
	std::vector<double> arr = { 1,2,3,4,5 };
	const auto idx=binary_search_find_index(arr,3.0);
	EXPECT_EQ(idx, 2);
}

TEST(binary_search, r_window_not_eq) {
	std::vector<double> arr = { 1,2,3,4,5 };
	const auto idx = binary_search_find_index(arr, 3.5);
	EXPECT_EQ(idx, 3);
}