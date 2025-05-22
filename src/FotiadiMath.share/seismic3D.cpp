#include "fotiadi_math.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <map>
#include <queue>
#include <chrono>
#include "graph_alg.h"
#include "fotiadi_math_private.h"
#include <omp.h>
#include "wave_alg.h"

class CorrelationObjectForTwoPoint {
private:
	std::vector<int16_t> forward;
	std::vector<int16_t> back;
public:
	CorrelationObjectForTwoPoint() :forward(0), back(0) {
	}
	CorrelationObjectForTwoPoint(
		const std::vector<int16_t>& _forward, const std::vector<int16_t>& _back):
		forward(_forward), back(_back){
	}
};

class MyEdgeS3: public EdgeCost<double> {
private:

	int32_t idx_corelation;
public:
	MyEdgeS3() :EdgeCost<double>(0,0, 0), idx_corelation(0) {
	}
	MyEdgeS3(const int32_t _idx_corelation,const double _cost, const int32_t _idx_start, const int32_t _idx_finish)
		:EdgeCost<double>(_idx_start,_idx_finish, _cost), idx_corelation(_idx_corelation) {}
	
	int32_t idx()const {
		return idx_corelation;
	}
};

class MyEdgeS3NoCost : public Edge {
private:
	int32_t idx_corelation;
public:
	MyEdgeS3NoCost() :Edge(0, 0), idx_corelation(0) {
	}
	MyEdgeS3NoCost(const int32_t _idx_corelation, const int32_t _idx_start, const int32_t _idx_finish)
		:Edge(_idx_start, _idx_finish), idx_corelation(_idx_corelation) {}

	int32_t idx()const {
		return idx_corelation;
	}
	void operator =(const MyEdgeS3& edge){
		(Edge)(*this) = (EdgeCost<double>)edge;
		idx_corelation = edge.idx();
	}
};

class ObjectCorrelation {
	private:
		const int32_t n_trace_x;
		const int32_t n_trace_y;
		const int32_t n_time;
		std::vector<CorrelationObjectForTwoPoint> correaltion;
		Graph<MyEdgeS3NoCost> graph;
public:
	ObjectCorrelation(
		const int32_t _n_trace_y,
		const int32_t _n_trace_x,
		const int32_t _n_time
	) :n_trace_x(_n_trace_x), n_trace_y(_n_trace_y), n_time(_n_time){
	}

	void init(const std::vector<CorrelationObjectForTwoPoint>& _correaltion,
		const std::vector<MyEdgeS3>& _edges) {
		Graph<MyEdgeS3> mst(n_trace_x * n_trace_y, _edges);
		Graph<MyEdgeS3>::kruskalMST(mst);
		const auto n = mst.edgeSize();
		std::vector<int32_t> set_correaltion(n);
		for (int32_t i = 0; i < n; i++) {
			set_correaltion[i] = mst.getEdge(i).idx();
		}
		std::sort(set_correaltion.begin(), set_correaltion.end());
		std::map<int32_t, int32_t> mapping;
		for (int32_t i = 0; i < n; i++) {
			mapping[set_correaltion[i]] = i;
		}
		correaltion = std::vector<CorrelationObjectForTwoPoint>(n);
		std::vector <MyEdgeS3NoCost> edges_mst(n);
		for (int32_t i = 0; i < n; i++) {
			correaltion[i] = _correaltion[set_correaltion[i]];
			edges_mst[i] = MyEdgeS3NoCost(mapping[mst.getEdge(i).idx()], mst.getEdge(i).idxStart(), mst.getEdge(i).idxEnd());
		}
		graph = Graph<MyEdgeS3NoCost>(n_trace_x * n_trace_y, edges_mst);
	}

	void trace(			
		const int32_t idx_trace_y,
		const int32_t idx_trace_x,			
		const int32_t idx_time,
		int32_t surface_idx[]){
		for (int32_t i = 0; i < idx_trace_y * idx_trace_x;i++) {
			surface_idx[i] = -1;
		}

		std::queue<std::tuple<int32_t, int32_t, int32_t>> queue;
		queue.push(std::make_tuple(idx_trace_y, idx_trace_x, idx_time));
		while (!queue.empty()) {
			auto cur_point = queue.front();
			queue.pop();
			int32_t idx_surface = std::get<0>(cur_point) * n_trace_x + std::get<1>(cur_point);
			surface_idx[idx_surface] = std::get<2>(cur_point);
			auto next_points = graph.near(idx_surface);
			for (const auto cur_next_point : next_points) {
				const int32_t idx_global = std::get<0>(cur_next_point);
				const int32_t idx_y = idx_global / n_trace_y;
				const int32_t idx_x = idx_global % n_trace_x;
				const MyEdgeS3NoCost& edge = graph.getEdge(std::get<1>(cur_next_point));
				if (surface_idx[idx_global]<0) {
					//edge. TODO
				}
			}
		}
	}

	/*void CalcSurface(const int32_t idx_y, const int32_t idx_x, const int32_t idx_time, int16_t surface[]) {
		const int32_t n=n_trace_y* n_trace_x;
		for (int32_t i = 0; i < n; i++) {
			surface[i] = std::numeric_limits<int16_t>::min();
		}
		std::queue < std::tuple<int32_t, int32_t, int16_t> > queue;
		queue.push({ idx_y ,idx_x,idx_time });
		while (!queue.empty()) {
			const auto cur_point = queue.front();
			queue.pop();
			//TO DO
			//surface[(std::get<0>(cur_point)* n_trace_x + std::get<1>(cur_point))*n_time + std::get<2>(cur_point)]=
		}

	}*/
};

inline const double* getSubArrayInMatrix(
	const int32_t n_trace_x, 
	const int32_t n_time,
	const double* signal, 
	const int32_t idx_trace_y, 
	const int32_t idx_trace_x) {
	return signal + ((n_time) * (n_trace_x * idx_trace_y + idx_trace_x));
}

inline int32_t getIdxForOpenMP(const int32_t i, const int32_t j, const int32_t n_trace_x,const int32_t num) {
	if (i == 0 && j == 0) {
		return num;
	}
	if (i == 0) {
		int32_t cur_idx = 4 + (j - 1) * 3;
		cur_idx += num  > 0 ? num - 1 : 0;
		return cur_idx;
	}
	if (j == 0) {
		int32_t cur_idx = (4 + (n_trace_x - 1) * 3) +  (i-1)*(3+(n_trace_x-1) * 2);
		cur_idx += num > 0 ? num-1 : 0;
		return cur_idx;
	}
	int32_t cur_idx = (4 + (n_trace_x - 1) * 3) + (i - 1) * (3 + (n_trace_x-1) * 2);
	cur_idx += 3 + (j - 1) * 2;
	cur_idx += num > 1 ?  num-2: 0;
	return cur_idx;
}

//!Создаем корреляционный объект
int32_t  FM_Seismic3DCorrelationByWaveCreateObject(
	const int32_t n_trace_y,			//!< количество трасс по оси X
	const int32_t n_trace_x,			//!< количество трасс по оси Y
	const int32_t n_time,				//!< количество временных отсчётов
	const double signals[],			    //!< сигналы [n_trace_y,n_trace_x,n_time]
	const int32_t max_shift_point_idx,	//!< Максимальный сдвиг в каждой точки по индексам между соседними трассами
	const int32_t n_fix_curves,         //!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const int16_t fix_point_idx[],		//!< -1 -> значит нет фиксации точки [n_trace_y,n_trace_x, n_fix_curves]
	const int16_t fix_point_idx_r[],    //!< для каждой точки задается окрестность в которой точку можно сдвинуть [n_trace_y,n_trace_x,n_fix_curves]
	CallBackProgress callback,			//!< обратная связь
	void** obj_correlation				//!< [out] Коды прослеженных пластов  [n_trace,n_time]
) {
	auto prev_time = std::chrono::high_resolution_clock::now();
	const int32_t min_deltatime_callback = 800;
	const auto duration_first = 0.9;
	bool cancel = false;

	int32_t idx_r = 10;//! посчитаем сами.
	std::vector<int16_t> x2y(n_time);
	std::vector<int16_t> y2x(n_time);
	double misfit;
	int32_t count_edge = 2 * n_trace_x * n_trace_y + n_trace_y + n_trace_x;
	std::vector<CorrelationObjectForTwoPoint> correaltion(count_edge);
	std::vector<MyEdgeS3> edges(count_edge);
	//int32_t counter = 0;

	const auto count_sectors = (n_trace_y - 1) * (n_trace_x - 1);
	#pragma omp parallel for schedule(static,1)
	for(int32_t idx=0;idx< count_sectors;idx++){
		auto cur_num=omp_get_thread_num();
		if (cancel) {
			continue;
		}
		int32_t i = idx / (n_trace_x - 1); 
		int32_t j = idx % (n_trace_x - 1);

		auto p00 = getSubArrayInMatrix(n_trace_x, n_time, signals,i,j);
		auto p10 = getSubArrayInMatrix(n_trace_x, n_time, signals, i+1, j);
		auto p01 = getSubArrayInMatrix(n_trace_x, n_time, signals, i, j+1);
		auto p11 = getSubArrayInMatrix(n_trace_x, n_time, signals, i+1, j+1);
		if (i == 0) {
			FM_CorrelationByWaveAlgorithmForTrace(
				n_time, p00, p01,
				max_shift_point_idx,
				0,nullptr,nullptr,nullptr, MetricFunctionSimpleConvolutionDouble,&idx_r,
				x2y.data(),y2x.data(), misfit);
			const auto idx0 = getIdxForOpenMP(i,j,n_trace_x,0);
			correaltion[idx0] = CorrelationObjectForTwoPoint(x2y, y2x);
			edges[idx0] = MyEdgeS3(idx0, misfit, i * n_trace_x + j, i * n_trace_x + (j + 1));
		}
		if (j == 0) {
			FM_CorrelationByWaveAlgorithmForTrace(
				n_time, p00, p10,
				max_shift_point_idx,
				0, nullptr, nullptr, nullptr, MetricFunctionSimpleConvolutionDouble, &idx_r,
				x2y.data(), y2x.data(), misfit);
			const auto idx1 = getIdxForOpenMP(i, j, n_trace_x, 1);
			correaltion[idx1] = CorrelationObjectForTwoPoint(x2y, y2x);
			edges[idx1] = MyEdgeS3(idx1,misfit, i * n_trace_x + j, (i + 1) * n_trace_x + j);
		}
		FM_CorrelationByWaveAlgorithmForTrace(
			n_time, p10, p11,
			max_shift_point_idx,
			0, nullptr, nullptr, nullptr, MetricFunctionSimpleConvolutionDouble, &idx_r,
			x2y.data(), y2x.data(), misfit);
		const auto idx2 = getIdxForOpenMP(i, j, n_trace_x, 2);
		correaltion[idx2] = CorrelationObjectForTwoPoint(x2y, y2x);
		edges[idx2] = MyEdgeS3(idx2, misfit, (i + 1) * n_trace_x + j, (i + 1) * n_trace_x + (j + 1));
			
		FM_CorrelationByWaveAlgorithmForTrace(
			n_time, p01, p11,
			max_shift_point_idx,
			0, nullptr, nullptr, nullptr, MetricFunctionSimpleConvolutionDouble, &idx_r,
			x2y.data(), y2x.data(), misfit);
		const auto idx3 = getIdxForOpenMP(i, j, n_trace_x, 3);
		correaltion[idx3] = CorrelationObjectForTwoPoint(x2y, y2x);
		edges[idx3] = MyEdgeS3(idx3, misfit, i * n_trace_x + (j + 1), (i + 1) * n_trace_x + (j + 1));
		//Обратная связь о прогрессе вычисления.
		//#pragma omp master
		if (omp_get_thread_num() == 0)
		{
			auto cur_time = std::chrono::high_resolution_clock::now();
			double elapsed_time_ms = std::chrono::duration<double, std::milli>(cur_time - prev_time).count();
			if (elapsed_time_ms > min_deltatime_callback) {
				if (callback != nullptr) {
					const auto progress = duration_first*((double)idx / (double)count_sectors);
					auto f_cb = callback(progress);
					if (f_cb != FM_CONTINUE_CALCULATE) {
						cancel = true;
					}
				}
				prev_time = cur_time;
			}
		}
	}
	if (cancel)
		return FM_ERROR_NO;
	auto f_cb = callback(duration_first);
	if (f_cb != FM_CONTINUE_CALCULATE) {
		cancel = true;
	}
	if (cancel)
		return FM_ERROR_NO;
	auto ptr_obj = new ObjectCorrelation(n_trace_y, n_trace_x, n_time);
	ptr_obj->init(correaltion, edges);
	*obj_correlation = ptr_obj;
    
	f_cb=callback(1.0);
	return FM_ERROR_NO;
}

int32_t  FM_Seismic3DCorrelationByWaveTraceBorders(
	//const int32_t n_trace_x,			//!< количество трасс по оси X
	//const int32_t n_trace_y,			//!< количество трасс по оси Y
	//const int32_t n_time,				//!< количество временных отсчётов
	const void* obj_correlation,  //!< объект в котором сохранены  
	const int32_t idx_trace_x,			//!< индекс трассы с которой будем прослеживать границу
	const int32_t idx_trace_y,			//!< индекс трассы с которой будем прослеживать границу
	const int32_t n_points,				//!< количесво точек для прослеживания
	const int32_t point_idx[],			//!< индексы точек которые надо проследить [n_points]
	const int32_t surface_idx[]		//!< [out] поверхности [n_points,n_trace_y,n_trace_x]
	) {
	const ObjectCorrelation* ptr_obj = (ObjectCorrelation*)obj_correlation;

	return FM_ERROR_NO;
}

int32_t FM_Seismic3DCorrelationByWaveDeleteObject(void* obj_correlation) {
	delete obj_correlation;
	return FM_ERROR_NO;
}

 int32_t FM_Seismic3DCorrelationByWaveSizeObject(const void* obj_correlation, int64_t & size_object) {
	return FM_ERROR_NO;
}

int32_t FM_Seismic3DCorrelationByWaveSerializeObject(const void* obj_correlation, uint8_t * data) {
	return FM_ERROR_NO;
}

int32_t FM_Seismic3DCorrelationByWaveRestoreObject(void** obj_correlation, uint8_t * data) {
	return FM_ERROR_NO;
}

//Вычисление функции сравнения с набором 
inline double MetricFunctionComparePointWithSetTrace(
	const int32_t n_time,
	const double signal_x[],
	const int32_t idx_x,
	const std::vector<const double*>& signal_y,
	const std::vector<int16_t>& surface_idx,
	int32_t r_idx
) {
	const auto count_y=signal_y.size();
	double sum = 0;
	int32_t counter = 0;
	for (int32_t i = 0; i < count_y; i++) {
		for (int32_t j = -r_idx; j <= r_idx; j++) {
			const auto cur_idx_x = idx_x + j;
			const auto cur_idx_y = surface_idx[i] + j;
			if ((0 <= cur_idx_x && cur_idx_x < n_time) && (0 <= cur_idx_y && cur_idx_y < n_time)) {
				const auto cur_add = signal_x[cur_idx_x] * signal_y[i][cur_idx_y];
				if (!std::isnan(cur_add)) {
					sum += cur_add;
					counter++;
				}
			}
		}
	}
	const double min_count = count_y*((r_idx * 2) / 3.0 - 0.5);
	if (0 == counter || counter < min_count) {
		return std::numeric_limits<double>::quiet_NaN();
	}
	auto res = sum / counter;
	return res;
}

int32_t defineIdxWithMinFunction(
	const int32_t n_time,
	const double signal_x[],
	const int32_t min_idx_x,
	const int32_t max_idx_x,
	const std::vector<const double*>& signal_y,
	const std::vector<int16_t>& surface_idx,
	const int32_t r_idx
) {
	int32_t idx_res = -1;
	double max_f_res= std::numeric_limits<double>::min();
	std::list<double> func;
	//#pragma omp parallel for
	for (int32_t i = min_idx_x; i <= max_idx_x;i++) {
		const auto cur_f = MetricFunctionComparePointWithSetTrace(n_time, signal_x,i, signal_y, surface_idx, r_idx);
		func.push_back(cur_f);
		if (cur_f < max_f_res) {
			#pragma omp critical
			{
				if (cur_f < max_f_res) {
					idx_res = i;
					max_f_res = cur_f;
				}
			}
		}
	}
	return idx_res;
}

int32_t  FM_Seismic3DTraceOneBorders(
	const int32_t n_trace_y,			//!< количество трасс по оси Y
	const int32_t n_trace_x,			//!< количество трасс по оси X
	const int32_t n_time,				//!< количество временных отсчётов
	const double signals[],			    //!< сигналы [n_trace_y,n_trace_x,n_time]
	const int32_t max_shift_point_idx,	//!< Максимальный сдвиг в каждой точки по индексам между соседними трассами         //!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	int16_t surface_idx[],				//!< -1 -> значит нет фиксации точки [n_trace_y,n_trace_x, n_fix_curves]
	CallBackProgress callback,			//!< обратная связь
	int32_t r_idx
) {
	//idx_y,idx_x, min_idx_border, max_idx_border
	std::queue<std::tuple<int32_t, int32_t, int32_t, int32_t>> queue;
	int32_t size_queue_iter = 0;
	//Инициализация массива
	MatrixMaze<int32_t, int16_t> grid_calc(n_trace_y, n_trace_x, -1);
	int32_t count_calc_pointer = 0;
	std::list<const double*> list_trace;
	std::list<int16_t> list_surface_idx;
	grid_calc.for_all_change(
		[&](int32_t i, int32_t j, int16_t cur_val) -> int16_t
		{
			const auto cur_val_surface = (surface_idx[i * n_trace_x + j]);
			if (cur_val_surface < 0) {
				//#pragma omp critical
				{
					count_calc_pointer++;
				}
			}
			else {
				grid_calc(i, j) = cur_val_surface;
				list_trace.push_back(&(signals[i * n_trace_x + j]));
				list_surface_idx.push_back(surface_idx[i * n_trace_x + j]);
			}
			return cur_val_surface;
		}
	);
	std::vector<const double* > vector_trace(list_trace.cbegin(), list_trace.cend());
	std::vector<int16_t> vector_surface_idx(list_surface_idx.cbegin(), list_surface_idx.cend());
	do{
		//_parallel
		grid_calc.for_all_use(
			[&](int32_t i, int32_t j, int16_t cur_val) -> void
			{
				if (cur_val < 0) {
					PointMazeDouble p_cur(i, j);
					std::array<PointMazeDouble, 8> neighbours;
					p_cur.getNeighbours(neighbours);
					int8_t count_neib = 0;
					int16_t min_idx_border = std::numeric_limits<int16_t>::max();
					int16_t max_idx_border = std::numeric_limits<int16_t>::min();
					for (int k = 0; k < 8; k++) {
						if ((0 <= neighbours[k].col() && neighbours[k].col() < n_trace_x) &&
							(0 <= neighbours[k].row() && neighbours[k].row() < n_trace_y)) {
							const auto cur_idx_border = grid_calc[neighbours[k]]; 
							if (cur_idx_border >= 0) {
								count_neib++;
								if (cur_idx_border < min_idx_border) {
									min_idx_border = cur_idx_border;
								}
								if (cur_idx_border > max_idx_border) {
									max_idx_border = cur_idx_border;
								}
							}
						}
					}
					if (count_neib > 0) {
						#pragma omp critical
						{
							queue.push(std::tie(i, j, min_idx_border, max_idx_border));
						}
					}
				}
			}
		);
		size_queue_iter = queue.size();
		while (!queue.empty()) {
			auto cur_point = queue.front();
			queue.pop();
			auto idx_trace_y = std::get<0>(cur_point);
			auto idx_trace_x= std::get<1>(cur_point);
			auto min_idx_cur_trace= std::get<2>(cur_point) - max_shift_point_idx;
			auto max_idx_cur_trace = std::get<3>(cur_point) + max_shift_point_idx;
			int16_t idx = defineIdxWithMinFunction(
				n_time,
				&(signals[(idx_trace_y*n_trace_x+ idx_trace_x)*n_time]),
				min_idx_cur_trace,
				max_idx_cur_trace,
				vector_trace,
				vector_surface_idx,
				r_idx);
			grid_calc(idx_trace_y, idx_trace_x) = idx;
		}
	} while (size_queue_iter);
	grid_calc.copy_to_array(surface_idx);
	return 0;
}

int32_t  FM_Seismic3DTraceBorders(
	const int32_t n_trace_y,			//!< количество трасс по оси Y
	const int32_t n_trace_x,			//!< количество трасс по оси X
	const int32_t n_time,				//!< количество временных отсчётов
	const double  signals[],			//!< сигналы [n_trace_y,n_trace_x,n_time]
	const int32_t max_shift_point_idx,	//!< Максимальный сдвиг в каждой точки по индексам между соседними трассами
	const int32_t nborders,				//!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	int16_t borders_idx[],				//!< -1 -> значит нет фиксации точки [n_trace_y, n_trace_x, n_fix_curves]
	CallBackProgress callback,			//!< обратная связь
	int r_idx
) {
	std::vector<int16_t> tmp(borders_idx+0, borders_idx+(n_trace_y* n_trace_x* nborders));
	for (int32_t i = 0; i < nborders; i++) {
		std::vector<int16_t> cur_surface(n_trace_y * n_trace_x);
		//#pragma omp parallel for
		for (int32_t idx = 0; idx < cur_surface.size(); idx++) {
			cur_surface[idx] = borders_idx[idx * nborders + i];
		}
		FM_Seismic3DTraceOneBorders(n_trace_y,n_trace_x,n_time, signals, max_shift_point_idx, cur_surface.data(), callback, r_idx);
		//#pragma omp parallel for
		for (int32_t idx = 0; idx < cur_surface.size(); idx++) {
			borders_idx[idx * nborders + i]= cur_surface[idx] ;
		}
	}
	return FM_ERROR_NO;
}