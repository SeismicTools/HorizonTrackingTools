#include <gtest/gtest.h>
#include "wave_alg.h"
TEST(WaveAlgorithm, DebugTest) {
	int32_t n = 3, m = 4;
	double matrix[] = { 
		1,2,3,4,
		1,2,3,4,
		1,2,3,4,
	};
	std::pair<int32_t, int32_t> start(0,0);
	std::pair<int32_t, int32_t> finish(2,3);
	double approximation = 1e6, approximation_bad_step = 1e+10;
	bool f_any = false;
	double diagParam = 1;
	std::list< std::pair<int, int> > path;
	auto res=WaveStdAlgMaze(n, m, matrix, approximation, start,  finish, f_any, &path, approximation_bad_step, diagParam);
	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}


TEST(WaveAlgMazeMultiEnds, DebugTest) {
	int32_t n = 3, m = 4;
	double matrix[] = {
		1,2,3,4,
		1,2,3,4,
		1,2,3,4,
	};
	MatrixMazeDouble similarity_matrix(n, m, matrix);
	PointMazeDouble start(0, 0);
	PointMazeDouble end1(2, 3);
	std::vector<PointMazeDouble> ends{ end1 };
	std::list< PointMazeDouble> path;

	const auto weight=WaveAlgMazeMultiEnds(similarity_matrix, start, ends, NextStepFunctionRightUpDown, nullptr, path);

	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}
