#pragma once

#include "new_list_maze.h"
#include "matrix_maze.h"

template <typename INT_T, typename FLOAT_T>
using NextStepFunctionTemplNewList = void (*)(
	const NewListMaze<FLOAT_T>& similarity_matrix,
	const PointMaze<INT_T>& point,
	const void* data,
	std::array<std::pair<PointMaze<INT_T>, FLOAT_T>, 8>& next_points,
	int8_t& count_points
	);

template <typename INT_T, typename FLOAT_T>
void NextStepFunctionRightDownTempl(
	const NewListMaze<FLOAT_T>& similarity_matrix,
	const PointMaze<INT_T>& point,
	const void* data,
	std::array<std::pair<PointMaze<INT_T>, FLOAT_T>, 8>& next_points,
	int8_t& count_points
) {
	count_points = 0;
	std::array<PointMaze<INT_T>, 8> neighbours;
	point.getNeighbours(neighbours);
	for (const auto& cur_next_point : neighbours) {
		if (similarity_matrix.isCorrectIndex<INT_T>(cur_next_point)) {
			auto cur_similarity = similarity_matrix[cur_next_point];
			if (!std::isnan(cur_similarity)
				&& cur_next_point.col() >= point.col()
				&& cur_next_point.row() >= point.row()) {
				const double mlt_coeff =
					cur_next_point.col() > point.col() && cur_next_point.row() > point.row() ? 1 : 1;
				next_points[count_points] = std::pair<PointMaze<INT_T>, double>(cur_next_point, mlt_coeff);
				count_points++;
			}
		}
	}
}

template <typename INT_T, typename FLOAT_T>
double WaveAlgMazeMultiEndsTempl(
	const NewListMaze<FLOAT_T>& similarity_matrix,
	const PointMaze<INT_T>& start,
	const std::vector<PointMaze<INT_T>>& ends,
	const NextStepFunctionTemplNewList<INT_T, FLOAT_T> f_step_function,
	const void* data_fns,
	std::list< PointMaze<INT_T>>& path,
	const INT_T max_shift_point_idx
)
{
	const FLOAT_T MaxValueWeight = 1e+20;
	path.clear();
	//Начальное состояние, максимальный вес.
	NewListMaze<FLOAT_T> matrix_weight
		(max_shift_point_idx, similarity_matrix.rows(), similarity_matrix.cols(), MaxValueWeight);
	//Очередь шагов
	std::queue< PointMaze<INT_T> > steps;
	//Инициализация первого шага
	steps.push(start);
	matrix_weight[start] = similarity_matrix[start];
	int64_t counter = 0;
	//Очередь для следующих шагов
	std::array<std::pair<PointMaze<INT_T>, FLOAT_T>, 8> next_steps;
	int8_t count_points;
	FLOAT_T cur_min_ends = MaxValueWeight;
	const auto period_for_check_min_ends = 5000;
	const auto start_checked = similarity_matrix.size();
	while (!steps.empty())
	{
		auto stp = steps.front();
		steps.pop();
		f_step_function(similarity_matrix, stp, data_fns, next_steps, count_points);
		for (int8_t i = 0; i < count_points; i++) {
			const auto& cur_next_step = next_steps[i];
			const auto mlt_coeff = cur_next_step.second;
			auto weight_cur_path = (matrix_weight[stp] + similarity_matrix[cur_next_step.first] * mlt_coeff);
			if (weight_cur_path < matrix_weight[cur_next_step.first] && weight_cur_path < cur_min_ends)
			{
				matrix_weight[cur_next_step.first] = weight_cur_path;
				steps.push(cur_next_step.first);
			}
		}
		counter++;
		//обновление лучшего текущего рекорда 
		if (0 == counter % period_for_check_min_ends && counter >= start_checked) {
			for (const auto& cur_end : ends) {
				cur_min_ends = std::min(cur_min_ends, matrix_weight[cur_end]);
			}
		}
	}
	//Находим завершение маршрута с минимальным весом 
	auto min_weight = MaxValueWeight;
	PointMaze<INT_T> min_end = start;
	for (const auto& cur_end : ends)
	{
		auto cur_weight = matrix_weight[cur_end];
		if (!std::isnan(cur_weight) && cur_weight < min_weight) {
			min_weight = cur_weight;
			min_end = cur_end;
		}
	}
	path.push_front(min_end);
	std::list<PointMaze<INT_T>> neighbours;
	while (path.front() != start) {
		const auto cur_point = path.front();
		cur_point.getNeighbours(neighbours);
		auto min_weight = MaxValueWeight;
		PointMaze<INT_T> min_next_point = start;
		for (const auto& cur_next_point : neighbours) {
			if (matrix_weight.isCorrectIndex(cur_next_point)) {
				auto cur_weight = matrix_weight[cur_next_point];
				if (!std::isnan(cur_weight) && cur_weight < min_weight) {
					min_weight = cur_weight;
					min_next_point = cur_next_point;
				}
			}
		}
		path.push_front(min_next_point);
	}
	return min_weight;
}

template <typename INT_T, typename FLOAT_T>
double WaveAlgMazeMultiPointsParallelTemplNewListMaze(
	const NewListMaze<FLOAT_T>& similarity_matrix,
	const std::vector<PointMaze<INT_T>>& start,
	const std::vector<PointMaze<INT_T>>& ends,
	const NextStepFunctionTemplNewList<INT_T, FLOAT_T> f_step_function,
	const void* data_fsf,
	std::list<PointMaze<INT_T>>& path,
	const INT_T max_shift_point_idx
)
{
	double min_f = std::numeric_limits<FLOAT_T>::max();
	std::list<PointMaze<INT_T>> min_path;
	#pragma omp parallel for
	for (INT_T i = 0; i < start.size(); i++) {
		std::list<PointMaze<INT_T>> cur_path;
		auto cur_f = WaveAlgMazeMultiEndsTempl<INT_T, FLOAT_T>
			(similarity_matrix, start[i], ends, f_step_function, data_fsf, cur_path, max_shift_point_idx);
	#pragma omp critical
		if (cur_f < min_f) {
			min_f = cur_f;
			min_path = cur_path;
		}
	}
	path.clear();
	path.splice(path.end(), min_path);
	return min_f;
}
