#include "wave_alg.h"

//Нужно добавить кейс когда пути нет. и можно подумать о ситуации когда путь до конца найден  это есть ограничение которое надо проверять
//Нужно пройти профилировщиком и проверить производительность алгоритма
double WaveAlgMazeMultiEnds(
	const MatrixMazeDouble& similarity_matrix,
	const PointMazeDouble& start,
	const std::vector<PointMazeDouble>& ends,
	const NextStepFunction f_next_step,
	const void* data_fns,
	std::list<PointMazeDouble>& path
) {
	const double MaxValueWeight = 1e+20;
	path.clear();
	//Начальное состояние, максимальный вес.
	MatrixMazeDouble matrix_weight(similarity_matrix.rows(), similarity_matrix.cols(), MaxValueWeight);
	//Очередь шагов
	std::queue< PointMazeDouble > steps;
	//Инициализация первого шага
	steps.push(start);
	matrix_weight[start] = similarity_matrix[start];
	int64_t counter = 0;
	//Очередь для следующих шагов
	std::array<std::pair<PointMazeDouble, double>, 8> next_steps;
	int8_t count_points;
	double cur_min_ends = MaxValueWeight;
	const auto period_for_check_min_ends = 5000;
	const auto start_checked = similarity_matrix.size();
	while (!steps.empty())
	{
		auto stp = steps.front();
		steps.pop();
		f_next_step(similarity_matrix, stp, data_fns, next_steps, count_points);
		for (int8_t i = 0; i < count_points;i++) {
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
			for (const auto& cur_end: ends) {
				cur_min_ends = std::min(cur_min_ends, matrix_weight[cur_end]);
			}
		}
	}
	//Находим завершение маршрута с минимальным весом 
	auto min_weight = MaxValueWeight;
	PointMazeDouble min_end = start;
	for(const auto& cur_end : ends)
	{
		auto cur_weight = matrix_weight[cur_end];
		if (!std::isnan(cur_weight) && cur_weight < min_weight) {
			min_weight = cur_weight;
			min_end = cur_end;
		}
	}
	path.push_front(min_end);
	std::list<PointMazeDouble> neighbours;
	while (path.front()!=start) {
		const auto cur_point = path.front();
		cur_point.getNeighbours(neighbours);
		auto min_weight = MaxValueWeight;
		PointMazeDouble min_next_point = start;
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


void NextStepFunctionRightUpDown(
	const MatrixMazeDouble& similarity_matrix,
	const PointMazeDouble& point,
	const void* data,
	std::array<std::pair<PointMazeDouble, double>, 8>& next_points,
	int8_t& count_points
) {
	count_points = 0;
	std::array<PointMazeDouble, 8> neighbours;
	point.getNeighbours(neighbours);
	for (const auto& cur_next_point : neighbours) {
		if (similarity_matrix.isCorrectIndex(cur_next_point)) {
			auto cur_similarity = similarity_matrix[cur_next_point];
			if (!std::isnan(cur_similarity) && cur_next_point.col() >= point.col()) {
				const double mlt_coeff = 1;
				next_points[count_points] = std::pair<PointMazeDouble, double>(cur_next_point, mlt_coeff);
				count_points++;
			}
		}
	}
}

void NextStepFunctionRightDown(
	const MatrixMazeDouble& similarity_matrix,
	const PointMazeDouble& point,
	const void* data,
	std::array<std::pair<PointMazeDouble, double>, 8>& next_points,
	int8_t& count_points
) {
	count_points = 0;
	std::array<PointMazeDouble, 8> neighbours;
	point.getNeighbours(neighbours);
	for (const auto& cur_next_point : neighbours) {
		if (similarity_matrix.isCorrectIndex(cur_next_point)) {
			auto cur_similarity = similarity_matrix[cur_next_point];
			if (!std::isnan(cur_similarity) 
				&& cur_next_point.col() >= point.col()
				&& cur_next_point.row() >= point.row()) {
				const double mlt_coeff =
					cur_next_point.col() > point.col() && cur_next_point.row() > point.row() ? 1 : 1;
				next_points[count_points] = std::pair<PointMazeDouble, double>(cur_next_point, mlt_coeff);
				count_points++;
			}
		}
	}
}

double WaveAlgMazeMultiPoints(
	const MatrixMazeDouble& similarity_matrix,
	const std::vector<PointMazeDouble>& start,
	const std::vector<PointMazeDouble>& ends,
	const NextStepFunction f_step_function,
	const void* data_fsf,
	std::list<PointMazeDouble>& path
) {
	double min_f = std::numeric_limits<double>::max();
	std::list<PointMazeDouble> min_path;
	for (int32_t i = 0; i < start.size(); i++) {
		std::list<PointMazeDouble> cur_path;
		auto cur_f=WaveAlgMazeMultiEnds(similarity_matrix, start[i], ends, f_step_function, data_fsf, cur_path);
		if (cur_f < min_f) {
			min_f = cur_f;
			min_path = cur_path;
		}
	}
	path.clear();
	path.splice(path.end(), min_path);
	return min_f;
}

double WaveAlgMazeMultiPointsParallel(
	const MatrixMazeDouble& similarity_matrix,
	const std::vector<PointMazeDouble>& start,
	const std::vector<PointMazeDouble>& ends,
	const NextStepFunction f_step_function,
	const void* data_fsf,
	std::list<PointMazeDouble>& path
) {
	double min_f = std::numeric_limits<double>::max();
	std::list<PointMazeDouble> min_path;
	#pragma omp parallel for
	for (int32_t i = 0; i < start.size();i++) {
		std::list<PointMazeDouble> cur_path;
		auto cur_f = WaveAlgMazeMultiEnds(similarity_matrix, start[i], ends, f_step_function, data_fsf, cur_path);
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