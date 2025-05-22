#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include "matrix_maze.h"

constexpr auto OUT_OF_MATRIX = -1;

template<typename FLOAT_T>
class NewListMaze
{
	private:
		std::vector<FLOAT_T> data; // значащие элементы матрицы
		std::vector<int32_t> idAtListUpRow; // id в data элементов верхней строки матрицы
		std::vector<int32_t> idAtListLeftCol; // id в data элементов левого столбца матрицы
		int32_t nrows, mcols, max_shift;

		// сопоставление id в vector`е data с самым верхним элементом диагонали
		// returns - число значащих элементов матрицы
		int32_t fillIdsOfDiagonals()
		{
			int shiftX = max_shift; // номер колонки (строка = 0), где начинается первая диагональ матрицы
			if (shiftX >= mcols)
			{
				shiftX = mcols - 1;
			}
			int shiftY = max_shift; // номер последней значащей строки. Последняя диагональ матрицы
			if (shiftY >= nrows)
			{
				shiftY = nrows - 1;
			}

			int32_t id = 0;
			int globalX = shiftX;
			// заполняем верхние диагонали
			for (int i = globalX; i >= 0; i--)
			{
				int x = i, y = 0;
				idAtListUpRow[x] = id;
				while (x < mcols && y < nrows)
				{
					//data.push_back(0);
					id++;
					x++;
					y++;
				}
			}

			// заполняем диагонали вниз
			for (int i = 1; i <= shiftY; i++)
			{
				int x = 0, y = i;
				idAtListLeftCol[y] = id;
				while (x < mcols && y < nrows)
				{
					//data.push_back(0);
					id++;
					x++;
					y++;
				}
			}

			return id;
		}

	public:
		inline int32_t rows()const { return this->nrows; }
		inline int32_t cols()const { return this->mcols; }
		inline int32_t size()const { return rows() * cols(); }

		inline bool isCorrectIndex(const int32_t _row, const int32_t _col)const {
			return (0 <= _row && _row < rows()) && (0 <= _col && _col < cols());
		}

		template <typename INT_T>
		inline bool isCorrectIndex(const PointMaze<INT_T>& idx)const {
			return isCorrectIndex(idx.row(), idx.col());
		}

		// заполнит значимые элементы матрицы нулями
		inline NewListMaze(const int max_shift, const int nrows, const int mcols)
			: NewListMaze(max_shift, nrows, mcols, 0)
		{
			
		}

		inline NewListMaze(const int max_shift, const int nrows, const int mcols, const FLOAT_T def_val)
			: nrows(nrows), max_shift(max_shift), mcols(mcols), idAtListUpRow(mcols, OUT_OF_MATRIX),
			idAtListLeftCol(nrows, OUT_OF_MATRIX)
		{
			int32_t numOfValuableNumbers = fillIdsOfDiagonals(); // количество элементов, которые будем хранить в матрице
			this->data = std::vector<FLOAT_T>(numOfValuableNumbers, def_val);
		}

		// проверяет, храним ли мы этот элемент матрицы
		// true - храним, значит он есть в data
		bool CheckIfInDiagonal(const int _row, const int _col)
		{
			return (std::abs(_row - _col) <= max_shift);
		}

		bool CheckIfInDiagonal(const int _row, const int _col) const
		{
			return (std::abs(_row - _col) <= max_shift);
		}

		bool CheckIfInDiagonal(const int _row, const int _col, const int maxShift)
		{
			return (std::abs(_row - _col) <= maxShift);
		}

		// получение элемента матрицы
		inline FLOAT_T operator() (const int _row, const int _col) const {
			if(!CheckIfInDiagonal(_row, _col))
			{
				constexpr FLOAT_T a = std::numeric_limits<FLOAT_T>::quiet_NaN();
				return a;
			}

			int minNum = std::min(_row, _col);
			std::pair<int, int> upRowCoord;
			upRowCoord = std::make_pair(_row-minNum, _col-minNum);
			if(upRowCoord.first == 0)
			{
				int idOfStart = idAtListUpRow[upRowCoord.second];
				int id = idOfStart + _col - upRowCoord.second;
				return data[id];
			}
			else
			{
				int idOfStart = idAtListLeftCol[upRowCoord.first];
				int id = idOfStart + _col - upRowCoord.second;
				return data[id];
			}
		} 

		inline FLOAT_T& operator() (const int _row, const int _col) {
			if (!CheckIfInDiagonal(_row, _col))
			{
				FLOAT_T a = std::numeric_limits<FLOAT_T>::quiet_NaN();
				return a;
			}

			int minNum = std::min(_row, _col);
			std::pair<int, int> upRowCoord;
			upRowCoord = std::make_pair(_row - minNum, _col - minNum);
			if (upRowCoord.first == 0)
			{
				int idOfStart = idAtListUpRow[upRowCoord.second];
				int id = idOfStart + _col - upRowCoord.second;
				return data[id];
			}
			else
			{
				int idOfStart = idAtListLeftCol[upRowCoord.first];
				int id = idOfStart + _col - upRowCoord.second;
				return data[id];
			}
		}

		template <typename INT_T>
		inline FLOAT_T operator[] (const PointMaze<INT_T>& idx) const {
			return (*this)(idx.row(), idx.col());
		}

		template <typename INT_T>
		inline FLOAT_T& operator[] (const PointMaze<INT_T>& idx) {
			return (*this)(idx.row(), idx.col());
		}

		// TODO: n.romanov OLD заполнение матрицы сделать через конструктор + lambda func + fillData удалить
		// заполнение матрицы след. образом (при nrows = 4, mcols = 4, max_shift = 1)
		// 3 0 0 0
		// 7 4 1 0
		// 0 8 5 2
		// 0 0 9 6 
		void fillData()
		{
			int shiftX = max_shift; // номер колонки (строка = 0), где начинается первая диагональ матрицы
			if(shiftX >= mcols)
			{
				shiftX = mcols-1;
			}
			int shiftY = max_shift; // номер последней значащей строки. Последняя диагональ матрицы
			if (shiftY >= nrows)
			{
				shiftY = nrows - 1;
			}

			int id = 0;
			int globalX = shiftX;
			// заполняем верхние диагонали
			for(int i = globalX; i>= 0; i--)
			{
				int x = i, y = 0;
				idAtListUpRow[x] = id;
				while(x < mcols && y < nrows)
				{
					data[id] = id;
					id++;
					x++;
					y++;
				}
			}

			// заполняем диагонали вниз
			for(int i = 1; i <= shiftY; i++)
			{
				int x = 0, y = i;
				idAtListLeftCol[y] = id;
				while(x < mcols && y < nrows)
				{
					data[id] = id;
					id++;
					x++;
					y++;
				}
			}
		}

		// elem = func(elem)
		void for_all_change_parallel(std::function<FLOAT_T(const FLOAT_T)> func) {
			const int32_t n = rows() * cols();
			#pragma omp parallel for
			for (int32_t idx = 0; idx < n; idx++) {
				const auto i = idx / cols();
				const auto j = idx % cols();
				(*this)(i, j) = func((*this)(i, j));
			}
		}

		// применяет функцию func ко всем значащим элементам матрицы (параллелит на процессоре)
		void for_all_change_parallel(std::function<FLOAT_T(const int32_t, const int32_t, const FLOAT_T)> func) {
			const int32_t n = rows() * cols();
			#pragma omp parallel for
			for (int32_t idx = 0; idx < n; idx++) {
				const auto i = idx / cols();
				const auto j = idx % cols();
				(*this)(i, j) = func(i, j, (*this)(i, j));
			}
		}

		//! Вычисление значений на основе матрицы
		void for_all_use(std::function<void(const FLOAT_T)> func) {
			for (int32_t i = 0; i < rows(); i++) {
				for (int32_t j = 0; j < cols(); j++) {
					func((*this)(i, j));
				}
			}
		}
};
