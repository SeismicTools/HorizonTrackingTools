#pragma once
#include <cstdint>

template<typename T>
static void GetIthArray(
	const T someArr[],		//! двумерный массив
	const int row_index,
	const int32_t size,		//! размер строки в двумерном массиве someArr
	T* sub_arr				//! [out]
)
{
	int start_index = row_index * size; // Начальный индекс строки в одномерном массиве
	int end_index = start_index + size; // Конечный индекс строки

	// Копирование элементов строки в подмассив
	for (int i = start_index, j = 0; i < end_index; i++, j++) {
		sub_arr[j] = someArr[i];
	}
}

template<typename T>
static void FillIthArray(
	T dest[],			//! двумерный массив
	const int num_line,		//! в какую по счету строку в dest вставляем source
	const T source[],
	const int lengthSource	//! длина строки в двумерном массиве dest
)
{
	for (int i = 0; i < lengthSource; i++)
	{
		dest[num_line * lengthSource + i] = source[i];
	}
}
