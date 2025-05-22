#include <cstdint>
#include <fotiadi_math.h>
#include "matrix_maze.h"
#include "correlation_alg.h"

namespace diff_trace_sizes
{
	int c_printTimes = 0;

	template <typename INT_T, typename FLOAT_T>
	void print(std::ostream& stream, std::list< PointMaze<INT_T>>* path) {
		for (const auto& cur_point : *path) {
			stream << cur_point.row() << ' ' << cur_point.col() << std::endl;
		}
	}

	// вывод данных по паре трасс (длина сигналов и сами сигналы)
	template <typename INT_T, typename FLOAT_T>
	void print(std::ostream& stream, 
		const INT_T n_grid_x, 
		const FLOAT_T signal_x[],
		const INT_T n_grid_y,
		const FLOAT_T signal_y[]
	) {
		stream << n_grid_x << std::endl;
		for(int i = 0; i < n_grid_x; i++)
		{
			stream << signal_x[i] << ' ';
		}
		stream << std::endl;
		stream << n_grid_y << std::endl;
		for (int i = 0; i < n_grid_y; i++)
		{
			stream << signal_y[i] << ' ';
		}
		stream << std::endl;
	}

	// нужно в dll вызвать её
	template <typename INT_T, typename FLOAT_T>
	int32_t  FM_CorrelationByWaveAlgorithmForTraceTempl(
		//!< количество сигналов на сетке X
		const INT_T n_grid_x,
		//!< количество сигналов на сетке X
		const INT_T n_grid_y,
		//!< сигналы на сетке X	[n_grid]
		const FLOAT_T signal_x[],
		//!< сигналы на сетке Y	[n_grid]
		const FLOAT_T signal_y[],
		//!< Максимальный сдвиг каждой точки по индексам
		const INT_T max_shift_point_idx,
		//!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
		const INT_T n_fix_borders,
		const INT_T fix_point_idx_x[],
		const INT_T fix_point_idx_y[],
		//!< для каждой точки задается окрестность в которой точку можно сдвинуть
		const INT_T fix_point_idx_r[],
		//!< Функция метрики 
		const MetricFunctionTempl<FLOAT_T, INT_T> fm,
		//!< Данные для функции метрики 
		const void* pdata_fm,
		//!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
		INT_T x2y[],
		//!< [out] значение индексов сетки X по сетки Y [n_interval]. Для каждой точки X выставлено значение по Y.
		INT_T y2x[],
		//!< [out] Значение целевой функции для маршрута. 
		FLOAT_T& target_function
	)
	{
	#ifdef _DEBUG
		if(!c_printTimes)
		{
			// сигналы двух трасс запишутся в txt файл + пара чисел с длиной массивов
			std::ofstream out("A_couple_traces.txt");
			print<INT_T, FLOAT_T>(out, n_grid_x, signal_x, n_grid_y, signal_y);
			out.close();
		}
	#endif

		MatrixMaze<INT_T, FLOAT_T> grid(n_grid_x, n_grid_y);
		if(n_grid_x == 1 || n_grid_y == 1)
		{
			return 0;
		}

		// сейчас в grid матрица заполнена нулями

		grid.fillDiagonal(
			max_shift_point_idx,
			n_fix_borders,
			fix_point_idx_x,
			fix_point_idx_y
		);

		//std::ofstream out2("matrix.txt");
		//grid.print(out2);
		//out2.close();

		// рассчет значений похожести
		grid.for_all_change_parallel(
			[&](const INT_T i, const INT_T j, const FLOAT_T cur_val) -> FLOAT_T
			{
				auto cur_fm = cur_val;
				if (!std::isnan(cur_val)) {
					cur_fm = fm(n_grid_x, signal_x, i, n_grid_y, signal_y, j, pdata_fm); // TODO: n.romanov здесь i и j менял местами, можно и в старой реализации поменять
				}
				return cur_fm;
			}
		);

		#ifdef _DEBUG
		if (!c_printTimes)
		{
			// вывод матрицы до нормализации (отрицательные минимальные значения = наиболее похожие сигналы)
			std::ofstream out("output1.txt");
			grid.print(out);
			out.close();
		}
		#endif

		// в norm_value запишется максимальное положительное значение из исходной матрицы - grid (если таковых нет, то некое малое положительное double число)
		const auto norm_value = NormMatrix<INT_T, FLOAT_T>(grid);

#ifdef _DEBUG
		if (!c_printTimes)
		{
			// вывод матрицы с нормализованными значениями [0;+inf). Меньше - более похоже
			std::ofstream out("A_jekowsky.txt");
			grid.print(out);
			out.close();
		}
#endif

		//Стартовые точки
		std::list<PointMaze<INT_T>> l_starts;
		std::list<PointMaze<INT_T>> l_ends;
		const auto cur_point_start_x = PointMaze<INT_T>(0, 0);
		const auto cur_point_finish_x = PointMaze<INT_T>(n_grid_x - 1, n_grid_y - 1);
		l_starts.push_back(cur_point_start_x);
		l_ends.push_back(cur_point_finish_x);
		
		//auto maxShift = max_shift_point_idx;
		//if (maxShift > grid.cols() || maxShift > grid.rows())
		//{
		//	maxShift = std::min(grid.cols(), grid.rows()) - 1;
		//}
		//for (int32_t i = 0; i <= maxShift; i++) {
		//	const auto cur_point_start_x = PointMaze<INT_T>(0, i);
		//	if (!std::isnan(grid[cur_point_start_x])) {
		//		l_starts.push_back(cur_point_start_x);
		//	}
		//	if (i != 0) {
		//		const auto cur_point_start_y = PointMaze<INT_T>(i, 0);
		//		if (!std::isnan(grid[cur_point_start_y])) {
		//			l_starts.push_back(cur_point_start_y);
		//		}
		//	}
		//	const auto cur_point_end_x = PointMaze<INT_T>(n_grid_x - 1, n_grid_y - 1 - i);
		//	if (!std::isnan(grid[cur_point_end_x])) {
		//		l_ends.push_back(cur_point_end_x);
		//	}
		//	if (i != 0) {
		//		const auto cur_point_end_y = PointMaze<INT_T>(n_grid_x - 1 - i, n_grid_y - 1);
		//		if (!std::isnan(grid[cur_point_end_y])) {
		//			l_ends.push_back(cur_point_end_y);
		//		}
		//	}
		//}

		std::vector<PointMaze<INT_T>> starts(l_starts.cbegin(), l_starts.cend());
		std::vector<PointMaze<INT_T>> ends(l_ends.cbegin(), l_ends.cend());
		// TODO: n.romanov стартовая и конечная точка 1
		std::list< PointMaze<INT_T>> path;
		target_function = WaveAlgMazeMultiPointsParallelTempl<INT_T, FLOAT_T>(grid, starts, ends, NextStepFunctionRightDownTempl<INT_T, FLOAT_T>, nullptr, path);
		target_function /= path.size();
		target_function -= norm_value;

		for (int32_t i = 0; i < n_grid_x; i++) {
			x2y[i] = std::numeric_limits<INT_T>::min();
		}

		for (int32_t i = 0; i < n_grid_y; i++)
		{
			y2x[i] = std::numeric_limits<INT_T>::min();
		}

#ifdef _DEBUG
		if (!c_printTimes)
		{
			// вывод координат пути в матрице
			std::ofstream pathFile("path.txt");
			print<INT_T, FLOAT_T>(pathFile, &path);
			pathFile.close();
		}
#endif

		// TODO: n.romanov здесь всё поменял, надо в старой реализации тоже поменять
		for (const auto& cur_point : path) {
			int idOnX = cur_point.row();
			int idOnY = cur_point.col();

			x2y[idOnX] = idOnY;
			y2x[idOnY] = idOnX;
		}

		c_printTimes = 1;

		return FM_ERROR_NO;
	}

	int32_t  FM_CorrelationByWaveAlgorithmForTrace(
		const int16_t n_grid_x,									//!< количество сигналов на сетке X 
		const int16_t n_grid_y,									//!< количество сигналов на сетке Y
		const double signal_x[],								//!< сигналы на сетке X	[n_grid]
		const double signal_y[],								//!< сигналы на сетке Y	[n_grid]
		const int16_t max_shift_point_idx,						//!< Максимальный сдвиг каждой точки по индексам
		const int16_t n_fix_borders,						    //!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
		const int16_t idsOfFixedPointsAtXTrace[],				//!< число фиксированных точек на одну трассу = число границ
		const int16_t idsOfFixedPointsAtYTrace[],				//!< 
		const int16_t fix_point_idx_r[],						//!< для каждой точки задается окрестность в которой точку можно сдвинуть
		const MetricFunctionDouble fm,							//!< Функция метрики 
		const void* pdata_fm,									//!< Данные для функции метрики 
		int16_t x2y[],											//!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
		int16_t y2x[],											//!< [out] значение индексов сетки X по сетки Y [n_interval]. Для каждой точки X выставлено значение по Y.
		double& target_function									//!< [out] Значение целевой функции для маршрута. 
	) {
		MetricFunctionTempl<double, int16_t> funcPtr = fm;
		return FM_CorrelationByWaveAlgorithmForTraceTempl<int16_t, double>(n_grid_x, n_grid_y,
			signal_x, signal_y, max_shift_point_idx, n_fix_borders, idsOfFixedPointsAtXTrace,
			idsOfFixedPointsAtYTrace, fix_point_idx_r, funcPtr, pdata_fm, x2y, y2x, target_function);
	}

	extern "C" EXPORT_FM int32_t FM_CorrelationByWaveAlgorithmFor2TracesWithDefaultMetricFunction(
		//!< количество сигналов на сетке X
		const int16_t n_grid_x,
		//!< количество сигналов на сетке X
		const int16_t n_grid_y,
		//!< сигналы на сетке X	[n_grid]
		const double signal_x[],
		//!< сигналы на сетке Y	[n_grid]
		const double signal_y[],
		//!< Максимальный сдвиг каждой точки по индексам - другими словами, ширина диагонали в корреляционной матрице
		const int16_t max_shift_point_idx,
		//!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
		const int16_t n_fix_borders,
		const int16_t fix_point_idx_x[],
		const int16_t fix_point_idx_y[],
		//!< Данные для функции метрики 
		const void* pdata_fm,
		//!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y.
		int16_t x2y[],
		//!< [out] значение индексов сетки X по сетки Y [n_interval]. Для каждой точки X выставлено значение по Y.
		int16_t y2x[],
		//!< [out] Значение целевой функции для маршрута. 
		double& target_function
	)
	{
		MetricFunctionTempl<double, int16_t> funcPtr = MetricFunctionSimpleConvolutionDouble;
		int16_t* _fix_point_idx_r = nullptr;
		return FM_CorrelationByWaveAlgorithmForTraceTempl<int16_t, double>(
			n_grid_x,
			n_grid_y,
			signal_x,
			signal_y,
			max_shift_point_idx,
			n_fix_borders,
			fix_point_idx_x,
			fix_point_idx_y,
			_fix_point_idx_r,
			funcPtr,
			pdata_fm,
			x2y,
			y2x,
			target_function
		);
	}
}
