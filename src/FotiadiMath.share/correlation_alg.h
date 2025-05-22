#pragma once

#include "wave_alg.h"
#include "new_list_maze.h"

//Нормализация матрицы: отрицательных сигналов не станет + максимальные значения станут минимальными и наоборот
template <typename SizeM, typename T>
T NormMatrix(MatrixMaze<SizeM, T>& grid) {
	//Поиск максимального значения
	T max_value = std::numeric_limits<T>::min();
	grid.for_all_use(
		[&](T cur_val) -> void
		{
			if (max_value < cur_val)
				max_value = cur_val;
		}
	);
	grid.for_all_change_parallel(
		[&](T cur_val) -> T
		{
			return -(cur_val - max_value) + 1e-100;
		}
	);
	return max_value;
}

//Нормализация матрицы без отрицательных значений
template <typename FLOAT_T>
FLOAT_T NormMatrixList(NewListMaze<FLOAT_T>& grid) {
	//Поиск максимального значения
	FLOAT_T max_value = std::numeric_limits<FLOAT_T>::min();
	grid.for_all_use(
		[&](FLOAT_T cur_val) -> void
		{
			if (max_value < cur_val)
				max_value = cur_val;
		}
	);
	grid.for_all_change_parallel(
		[&](FLOAT_T cur_val) -> FLOAT_T
		{
			return -(cur_val - max_value) + 1e-100;
		}
	);
	return max_value;
}
