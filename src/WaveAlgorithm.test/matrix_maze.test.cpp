#include <gtest/gtest.h>
#include "matrix_maze.h"

namespace matrixMazeTests
{
	TEST(MatrixMaze, for_all_use) {
		const double etalon = 30;
		int32_t n = 3, m = 4;
		double matrix[] = {
			1,2,3,4,
			1,2,3,4,
			1,2,3,4,
		};
		MatrixMazeDouble similarity_matrix(n, m, matrix);
		double sum = 0;
		similarity_matrix.for_all_use(
			[&](double cur_val) -> void
			{
				sum += cur_val;
			}
		);
		EXPECT_NEAR(sum, etalon, 1e-10);
	}

	TEST(MatrixMaze, for_all_change) {
		const double etalon = 30;
		int32_t n = 3, m = 4;
		double matrix[] = {
			1,2,3,4,
			1,2,3,4,
			1,2,3,4,
		};
		MatrixMazeDouble similarity_matrix(n, m, matrix);

		similarity_matrix.for_all_change(
			[](double cur_val) -> double
			{
				return -cur_val;
			}
		);

		double sum = 0;
		similarity_matrix.for_all_use(
			[&](double cur_val) -> void
			{
				sum += cur_val;
			}
		);

		EXPECT_NEAR(sum, -etalon, 1e-10);
	}
}
