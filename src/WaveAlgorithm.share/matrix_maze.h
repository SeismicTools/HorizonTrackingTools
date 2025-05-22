#pragma once

#include<cstdint>
#include<array>
#include<memory>
#include<vector>
#include<utility>
#include<ostream>
#include<functional>
#include<list>

//точка в лабиринте
template<typename SizeM>class PointMaze {
private:
	std::pair<SizeM, SizeM> point;
public:
	inline PointMaze() :point(0,0) {}
	inline PointMaze(const SizeM row, const SizeM col) :point(row, col) {}
	inline PointMaze<SizeM>& operator =(const PointMaze<SizeM>& _p) { 
		this->point = _p.point; 
		return *this; 
	}
	inline bool operator==(const PointMaze<SizeM>& _p)const { return this->point == _p.point; }
	inline bool operator!=(const PointMaze<SizeM>& _p)const { return !(*this == _p); }
	inline SizeM row()const { return point.first; }
	inline SizeM col()const { return point.second; }

	void getNeighbours(std::list<PointMaze<SizeM>>& neighbours)const {
		const PointMaze<SizeM> next[] = {
			PointMaze<SizeM>(row() - 1, col() - 1),
			PointMaze<SizeM>(row() - 1, col()),
			PointMaze<SizeM>(row() - 1, col() + 1),

			PointMaze<SizeM>(row(), col() - 1),
			PointMaze<SizeM>(row(), col() + 1),

			PointMaze<SizeM>(row() + 1, col() - 1),
			PointMaze<SizeM>(row() + 1, col()),
			PointMaze<SizeM>(row() + 1, col() + 1)
		};
		constexpr auto size_next = sizeof(next) / sizeof(PointMaze<SizeM>);
		neighbours.clear();
		neighbours.insert(neighbours.end(), next,next+ size_next);
	}

	void getNeighbours(std::array<PointMaze<SizeM>,8>& neighbours)const {
		neighbours[0] = PointMaze<SizeM>(row() - 1, col() - 1);
		neighbours[1] = PointMaze<SizeM>(row() - 1, col());
		neighbours[2] = PointMaze<SizeM>(row() - 1, col() + 1);

		neighbours[3] = PointMaze<SizeM>(row(), col() - 1);
		neighbours[4] = PointMaze<SizeM>(row(), col() + 1);

		neighbours[5] = PointMaze<SizeM>(row() + 1, col() - 1);
		neighbours[6] = PointMaze<SizeM>(row() + 1, col());
		neighbours[7] = PointMaze<SizeM>(row() + 1, col() + 1);
	}
};

using PointMazeDouble = PointMaze<int32_t>;

//Матрица с n-строк и m-столбцов
template<typename SizeM,typename T>class MatrixMaze {
private:
	SizeM nrows, mcols;
	std::vector<T> data;
private:
	MatrixMaze();
public:
	inline MatrixMaze(const SizeM _rows,const SizeM _cols,const T * const _ptr)
		:nrows(_rows), mcols(_cols), data(_ptr,_ptr+(_rows* _cols)) {}

	inline MatrixMaze(const SizeM _rows, const SizeM _cols, const T def_val)
		: nrows(_rows), mcols(_cols), data(_rows* _cols, def_val){}
	inline MatrixMaze(const SizeM _rows, const SizeM _cols)
		: nrows(_rows), mcols(_cols), data(_rows* _cols) {}
public:

	inline T& operator() (const SizeM _row,const SizeM _col) {
		if(_row >= this->nrows || _col >= this->mcols)
		{
			throw std::exception();
			//return data[0];
		}
		auto idToSet = _row * cols() + _col;
		return data[idToSet];
	}
	inline T  operator() (const SizeM _row,const SizeM _col) const {
		return data[_row*cols() + _col];
	}
	inline T& operator[] (const PointMazeDouble& idx) {
		return data[idx.row() * cols() + idx.col()];
	}
	inline T  operator[] (const PointMazeDouble& idx) const {
		return data[idx.row() * cols() + idx.col()];
	}
	template <typename INT_T>
	inline T& operator[] (const PointMaze<INT_T>& idx) {
		return data[idx.row() * cols() + idx.col()];
	}
	template <typename INT_T>
	inline T  operator[] (const PointMaze<INT_T>& idx) const {
		return data[idx.row() * cols() + idx.col()];
	}

	inline SizeM rows()const { return this->nrows; }
	inline SizeM cols()const { return this->mcols; }
	inline SizeM size()const { return rows() * cols(); }

	inline bool isCorrectIndex(const SizeM _row, const SizeM _col)const {
		return (0 <= _row && _row < rows()) && (0 <= _col && _col < cols());
	}
	inline bool isCorrectIndex(const PointMazeDouble& idx)const {
		return isCorrectIndex(idx.row(), idx.col());
	}
	template <typename INT_T>
	inline bool isCorrectIndex(const PointMaze<INT_T>& idx)const {
		return isCorrectIndex(idx.row(), idx.col());
	}
	//! Вычисление значений на основе матрицы
	void for_all_use(std::function<void(const T)> f) {
		for (SizeM i = 0; i < rows(); i++) {
			for (SizeM j = 0; j < cols(); j++) {
				f((*this)(i, j));
			}
		}
	}

	void for_all_use_parallel(std::function<void(const T)> f) {
		const auto n = rows() * cols();
		#pragma omp parallel for
		for (SizeM idx = 0; idx < n; idx++) {
			const auto i = idx / cols();
			const auto j = idx % cols();
			(*this)(i, j) = f((*this)(i, j));
		}
	}

	void for_all_use(std::function<void(const SizeM,const SizeM,const T)> f) {
		for (SizeM i = 0; i < rows(); i++) {
			for (SizeM j = 0; j < cols(); j++) {
				f(i,j,(*this)(i, j));
			}
		}
	}
	void for_all_use_parallel(std::function<void(const SizeM,const SizeM,const T)> f) {
		#pragma omp parallel for
		for (SizeM i = 0; i < rows(); i++) {
			for (SizeM j = 0; j < cols(); j++) {
				f(i, j, (*this)(i, j));
			}
		}
	}

	//! Поэлементное изменение матрицы 
	void for_all_change(std::function<T(const T)> f) {
		for (SizeM i = 0; i < rows(); i++) {
			for (SizeM j = 0; j < cols(); j++) {
				(*this)(i, j)=f((*this)(i, j));
			}
		}
	}

	void for_all_change_parallel(std::function<T(const T)> f) {
		const int32_t n = rows() * cols();
		#pragma omp parallel for
		for (int32_t idx = 0; idx < n; idx++) {
			const auto i = idx / cols();
			const auto j = idx % cols();
			(*this)(i, j) = f((*this)(i, j));
		}	
	}

	bool areEqual(double a, double b, double epsilon = 1e-9) {
		return std::fabs(a - b) < epsilon;
	}

	// SizeM = int
	void fillDiagonal(
		SizeM max_shift_point_idx,
		SizeM n_fix_borders,
		const SizeM fix_point_idx_x[],
		const SizeM fix_point_idx_y[]
	)
	{
		T nanValue = std::numeric_limits<T>::quiet_NaN(); // значения за диагональю равны null
		for(int i = 0; i < nrows; i++)
		{
			for(int j = 0; j < mcols; j++)
			{
				(*this)(i, j) = nanValue;
			}
		}

		double step = ((double)nrows-1.0) / ((double)mcols-1.0); // шаг по rows, чтобы на последнем шаге оказаться в нижнем правом углу
		double secondOneI = std::ceil(step); // на второй итерации i будет равна этому числу (это макс расстояние по rows между двумя точками)
		double cellsBetween = secondOneI - 1.0; // max count клеток между двумя точками
		double width;
		if(cellsBetween - (double)max_shift_point_idx * 2.0 > 0.0)
		{
			width = std::ceil(cellsBetween / 2.0);
		}
		else
		{
			width = (double)max_shift_point_idx;
		}

		double lastOneRow = (double)nrows - 1.0;
		int j = 0;
		int widthInt = (int)width;
		for(double i = 0.0; true; i += step, j++)
		{
			int intI = (int)i;
			for(int rowIdToFill = intI - widthInt; rowIdToFill <= intI + widthInt; rowIdToFill++)
			{
				if(rowIdToFill >= 0 && rowIdToFill < nrows)
					(*this)(rowIdToFill, j) = 0;
			}

			if(areEqual(i, lastOneRow))
			{
				break;
			}
		}

		// фиксирую границы
		for(int i = 0; i < n_fix_borders; i++)
		{
			int rowNum = fix_point_idx_x[i];
			int colNum = fix_point_idx_y[i];

			if(rowNum >= nrows || colNum >= mcols)
			{
				continue;
			}

			if(rowNum < 0 || colNum < 0)
				continue;

			// заполнение верхнего правого прямоуг

			auto rowNumToSet = 0;
			auto colNumToSet = colNum + 1;
			for (; rowNumToSet < rowNum && colNumToSet != mcols;)
			{
				set_value(rowNumToSet, colNumToSet, nanValue);
				colNumToSet++;
				if (colNumToSet == mcols)
				{
					colNumToSet = colNum + 1;
					rowNumToSet++;
				}
			}
			// заполнение нижнего левого прямоуг
			colNumToSet = 0;
			rowNumToSet = rowNum + 1;
			for (; rowNumToSet < nrows && colNumToSet != colNum;)
			{
				set_value(rowNumToSet, colNumToSet, nanValue);
				colNumToSet++;
				if (colNumToSet == colNum)
				{
					colNumToSet = 0;
					rowNumToSet++;
				}
			}
		}
	}

	void for_all_change(std::function<T(const SizeM,const SizeM,const T)> f) {
		for (SizeM i = 0; i < rows(); i++) {
			for (SizeM j = 0; j < cols(); j++) {
				(*this)(i, j) = f(i,j,(*this)(i, j));
			}
		}
	}
	
	// заполнение матрицы данными
	void for_all_change_parallel(std::function<T(const SizeM, const SizeM,const T)> f) {
		auto rowsCount = rows();
		auto colsCount = cols();
		const int32_t n = rowsCount * colsCount;
		#pragma omp parallel for
		for (int32_t idx = 0; idx < n; idx++) {
			const auto i = idx / cols();
			const auto j = idx % cols();
			auto calculatedValue = f(i, j, (*this)(i, j));
			(*this)(i, j) = calculatedValue;
		}
	}

	void set_value(int32_t i, int32_t j, T value)
	{
		if(i >= 0 && i < nrows && j >= 0 && j < mcols)
			(*this)(i, j) = value;
	}

	void for_all_change(std::function<T(const SizeM,const SizeM)> f) {
		for (SizeM i = 0; i < rows(); i++) {
			for (SizeM j = 0; j < cols(); j++) {
				(*this)(i, j) = f(i, j);
			}
		}
	}

	void for_all_change_parallel(std::function<T(const SizeM,const SizeM)> f) {
		const auto n = rows() * cols();
		#pragma omp parallel for
		for (SizeM idx = 0; idx < n; idx++) {
			const auto i = idx / cols();
			const auto j = idx % cols();
			(*this)(i, j) = f(i, j);
		}
	}
	
	void copy_to_array(T to_array[]) {
		std::memcpy(to_array, data.data(), data.size() * sizeof(T));
	}

	void print(std::ostream& stream) {
		for (SizeM i = 0; i < rows(); i++) {
			for (SizeM j = 0; j < cols(); j++) {
				stream << (*this)(i,j) <<"\t";
			}
			stream << std::endl;
		}
	}
};

using MatrixMazeDouble = MatrixMaze<int32_t,double>;

using MatrixMazeFloat = MatrixMaze<uint16_t, float>;

