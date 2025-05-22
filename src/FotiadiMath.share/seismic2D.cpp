#include "fotiadi_math.h"
#include <vector>
#include <unordered_map>
#include <string>
#include "fotiadi_util.h"

/*
* Сохраняет в myMap отображение (индекс, куда отобразился left trace на right trace -> id границы для него)
*/
static void Fill_trace_codes_initital(double* mapping_of_traces, int32_t size, std::unordered_map<int, int>& myMap, int32_t& border_id)
{
	for(int i = 0; i < size; i++)
	{
		if (myMap.find(mapping_of_traces[i]) != myMap.end()) { // уже есть в hashMap
			// myMap[key]
		}
		else {
			myMap[mapping_of_traces[i]] = border_id;
			border_id++;
		}
	}
}

/*
* Заполняет newMap, ориентируясь на результаты корреляции прошлых трасс
*/
static void Fill_trace_codes(double* mapping_old, int32_t size, double* mapping_new, std::unordered_map<int, int> old_map, std::unordered_map<int, int>& newMap, int32_t& border_id)
{
	for(int i = 0; i < size; i++) // проставляю уже заданные номера границ, известные с прошлого прохода алгоритма
	{
		if (newMap.find(mapping_new[(int32_t)mapping_old[i]]) == newMap.end()) { // еще нет в hashMap
			newMap[mapping_new[(int32_t)mapping_old[i]]] = old_map[i];
		}
	}

	for(int i = 0; i < size; i++) // добавляю новые границы, если где-то они не отмечены
	{
		if (newMap.find(mapping_new[i]) != newMap.end()) { // уже есть в hashMap
			// myMap[key]
		}
		else {
			newMap[mapping_new[i]] = border_id;
			border_id++;
		}
	}
}

static void Fill_border_results(int32_t* grid_y_by_x, int32_t size, int trace_number, std::unordered_map<int, int> myMap, double* mapping)
{
	for(int i = 0; i < size; i++)
	{
		// TODO: n.romanov OLD ошибка, записываю границу в id left трассы, хотя это номер границы right трассы
		grid_y_by_x[trace_number * size + i] = myMap[mapping[i]];
	}
}

int32_t  FM_Seismic2DCorrelationByWaveAlgorithm( 
	const int32_t n_trace,				//!< количество трасс 
	const int32_t n_time,				//!< количество временных отсчётов
	const double signals[],			    //!< сигналы [n_trace,n_time]
	const int32_t max_shift_point_idx,	//!< Максимальный сдвиг в каждой точке по индексам между соседними трассами
	const int32_t n_fix_curves,          //!< Количество зафиксированных точек, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const int32_t fix_point_idx[],		//!< -1 -> значит нет фиксации точки   [n_trace,n_fix_point]
	const int32_t fix_point_idx_r[],    //!< для каждой точки задается окрестность в которой точку можно сдвинуть
	const MetricFunction fm,			//!< Функция метрики 
	const void* pdata_fm,				//!< Данные для функции метрики 
	int32_t grid_y_by_x[]              //!< [out] Коды прослеженных пластов  [n_trace,n_time]
) {
	if (n_trace <= 1 || n_time < 1) {
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}
	std::vector<double> sub_signals_l(n_time);
	std::vector<double> sub_signals_r(n_time);

	std::vector<int32_t> fix_point_idx_left(n_fix_curves);
	std::vector<int32_t> fix_point_idx_right(n_fix_curves);
	std::unordered_map<int, int> myMap; // отображение индекса трассы в индекс границы
	std::vector<double> last_trace_result(n_time); // результат отображения на прошлой итерации 
	int32_t border_id = 1;

	for(int i = 0; i < n_trace-1; i++)
	{
		if(i == 0)
		{
			GetIthArray<double>(signals, i, n_time, sub_signals_l.data());
			GetIthArray<int32_t>(fix_point_idx, i, n_fix_curves, fix_point_idx_left.data());
		}
		GetIthArray<double>(signals, i+1, n_time, sub_signals_r.data());
		GetIthArray<int32_t>(fix_point_idx, i+1, n_fix_curves, fix_point_idx_right.data());

		std::vector<double> result(n_time); // здесь будет результат корреляции двух трасс

		double tf;
		FM_CorrelationByWaveAlgorithm(n_time, 0, sub_signals_l.data(),
			n_time, 0, sub_signals_r.data(),
			n_time, max_shift_point_idx, 
			n_fix_curves, fix_point_idx_left.data(), fix_point_idx_right.data(), fix_point_idx_r,
			fm, pdata_fm, result.data(), tf);
			// 193 -> 194
		if(i == 0)
		{
			Fill_trace_codes_initital(result.data(), n_time, myMap, border_id);
			Fill_border_results(grid_y_by_x, n_time, i, myMap, result.data());
		}
		else
		{
			std::unordered_map<int, int> newMap;
			Fill_trace_codes(last_trace_result.data(), n_time, result.data(), myMap, newMap, border_id);
			Fill_border_results(grid_y_by_x, n_time, i, myMap, result.data());
			myMap = newMap;
		}

		last_trace_result = result;

		sub_signals_l = sub_signals_r;
		fix_point_idx_left = fix_point_idx_right;
	}

	// у result все индексы сдвинуты на 1 вправо

	// сейчас в sub_signals_l и в fix_point_idx_left лежит последняя трасса
	GetIthArray<double>(signals, n_trace-2, n_time, sub_signals_r.data()); // в правый считал предпоследнюю
	GetIthArray<int32_t>(fix_point_idx, n_trace - 2, n_fix_curves, fix_point_idx_right.data());

	std::vector<double> result(n_time); // здесь будет результат корреляции двух трасс
	double tf;
	// делаем отображение правой трассы в левую
	FM_CorrelationByWaveAlgorithm(n_time, 0, sub_signals_r.data(),
		n_time, 0, sub_signals_l.data(),
		n_time, max_shift_point_idx,
		n_fix_curves, fix_point_idx_right.data(), fix_point_idx_left.data(), fix_point_idx_r,
		fm, pdata_fm, result.data(), tf);
	
	for(int i = 0; i < n_time; i++)
	{
		if(result[i] >= 0 && result[i] < last_trace_result.size() && result[i] == last_trace_result[result[i]])
		{
			grid_y_by_x[(n_trace-1) * n_time + i] = myMap[last_trace_result[result[i]]];
		}
		else
		{
			grid_y_by_x[(n_trace - 1) * n_time + i] = -1;
		}
	}


	return FM_ERROR_NO;
}
