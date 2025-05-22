#include <gtest/gtest.h>
#include "list_maze.h"
#include "new_list_maze.h"

namespace listMazeTests
{
	static TEST(ListMaze, list_creation) {
		ListMaze<double> listMaze(1, 3, 11);
		auto value = listMaze(1, 5);
		EXPECT_EQ(value, 16.0);
	}

	static TEST(ListMaze, list_creation1) {
		ListMaze<double> listMaze(1, 10, 10);
		auto value = listMaze(4, 3);
		EXPECT_EQ(value, 43.0);
	}
	static TEST(ListMaze, list_creation2) {
		ListMaze<double> listMaze(1, 10, 10);
		auto value = listMaze(0, 0);
		EXPECT_EQ(value, 0);
	}
	static TEST(ListMaze, list_creation3) {
		ListMaze<double> listMaze(1, 10, 10);
		auto value = listMaze(9, 9);
		EXPECT_EQ(value, 99);
	}

	static TEST(ListMaze, getElemBinSearch) {
		ListMaze<double> listMaze(1, 3, 11);
		auto value = listMaze.getElem(1, 5);
		EXPECT_EQ(value, 16.0);
	}

	static TEST(ListMaze, getElemBinSearch1) {
		ListMaze<double> listMaze(1, 10, 10);
		auto value = listMaze.getElem(4, 3);
		EXPECT_EQ(value, 43.0);
	}
	static TEST(ListMaze, getElemBinSearch2) {
		ListMaze<double> listMaze(1, 10, 10);
		auto value = listMaze.getElem(0, 0);
		EXPECT_EQ(value, 0);
	}
	static TEST(ListMaze, getElemBinSearch3) {
		ListMaze<double> listMaze(1, 10, 10);
		auto value = listMaze.getElem(9, 9);
		EXPECT_EQ(value, 99);
	}

	static TEST(ListMaze, getElemBinSearch4) {
		ListMaze<double> listMaze(1, 10, 10);
		auto value = listMaze.getElem(9, 0);
	}

	static TEST(NewListMaze, getElem) {
		NewListMaze<double> listMaze(1, 3, 3);
		listMaze.fillData();

		const double value1 = listMaze(2, 0);
		EXPECT_TRUE(std::isnan(value1));
		double value = listMaze(1, 1);
		EXPECT_EQ(value, 3);
		value = listMaze(1, 2);
		EXPECT_EQ(value, 1);
		value = listMaze(1, 0);
		EXPECT_EQ(value, 5);
	}

	static TEST(NewListMaze, getElem1) {
		NewListMaze<double> listMaze(1, 4, 4);
		listMaze.fillData();
		auto value1 = listMaze(3, 2);
		EXPECT_EQ(value1, 9);
	}

	// всем значащим значениям добавляем 5, NaN`ы не трогает
	static TEST(NewListMaze, for_parallel_test)
	{
		NewListMaze<double> listMaze(1, 4, 4);
		listMaze.for_all_change_parallel(
			[&](const int32_t i, const int32_t j, const double cur_val) -> double
			{
				if(!std::isnan(cur_val))
				{
					return cur_val+5;
				}
				return cur_val;
			});
		for(int x = 0; x < listMaze.rows(); x++)
		{
			for (int y = 0; x < listMaze.cols(); x++)
			{
				if(listMaze.CheckIfInDiagonal(x, y))
				{
					EXPECT_EQ(listMaze(x, y), 5);
				}
				else
				{
					EXPECT_TRUE(std::isnan(listMaze(x, y)));
				}
			}
		}
	}
}
