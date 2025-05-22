/** @file fotiadi_math.h
 * Основные функции библиотеки.
 */

#pragma once

#include <cstdint>
#include <string>

#if defined(_MSC_VER)
//  Microsoft 
#ifdef __EXPORT_FUNCTION__
#define EXPORT_FM __declspec(dllexport)
#else
#define EXPORT_FM __declspec(dllimport)
#endif
#elif defined(__GNUC__)
//  GCC
#ifdef __EXPORT_FUNCTION__
#define EXPORT_FM __attribute__((visibility("default")))
#else
#define EXPORT_FM 
#endif
#else
//  do nothing and hope for the best?
#define EXPORT_FM
#pragma warning Unknown dynamic link import/export semantics.
#endif

#define FM_ERROR_NO	                      0
#define FM_CALCULATIONS_STOPPED_BY_UI	  -22
//! Входные параметры функции не верные
#define FM_ERROR_PARAMETRS_NOT_VALID     -11

#define FM_ERROR_NO_SOLUTION_FOUND       -777

//! Функция, возвращающая прогресc вычисления алгоритма.
/// @param[in]	 progress	- текущий прогресс [0..1], 0 - задача не сделана, 1 - задача выполонена
/// @return					0 - продолжить вычисления вычисления, 1 - остановить вычисления
typedef int32_t(*CallBackProgress)(const double progress);

#define FM_CONTINUE_CALCULATE 0
#define FM_BREAK_CALCULATE 1
/*!
* @brief Подготовка данных для корреляции (для волнового алгоритма).
* 
* Переводит значения на индексы.
*/
extern "C" EXPORT_FM int32_t  FM_PrepareDataForCorrelation(
	const int32_t n_signal_init_x,			//!< Количество отсчетов эксперементальных данных 
	const double grid_init_x[],				//!< Сетка времен эксперементальных данных [n_signal_init_x]
	const double signal_init_x[],			//!< Значение эксперементальных данных [n_signal_init_x]
	const int32_t n_signal_init_y,			//!< Количество отсчетов синтетических данных 
	const double grid_init_y[],				//!< Сетка синтетических данных [n_signal_init_y]
	const double signal_init_y[],			//!< Значение синтетических данных [n_signal_init_x]
	const int32_t n_fix_point_init,         //!< Количество зафиксированных точек, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const double fix_point_init_x[],
	const double fix_point_init_y[],
	const double fix_point_init_r[],
	const double start_interval,			//!< Начало интервала для корреляции по времени, мс 
	const double finish_interval,			//!< Окончание интервала корреляции по времени, мс  
	const double step_grid,					//!< Шаг генерируемой сетки, мс 
	const double max_shift,					//!< Максимальный сдвиг синтетических данных относительно эксперимента, мс 
	int32_t & n_interval,					//!< Количество элементов в коррелируемом интервале
	const int32_t n_signal_x,
	int32_t & n_signal_idx_x,				//!< количество сигналов на сетке X, <=n_signal_x
	int32_t & shift_idx_x,					//!< индекс начала интервала коррелирования
	double signal_idx_x[],					//!< сигнал X [n_signal_x], количесво значимых элементов n_signal_idx_x
	const int32_t n_signal_y,				//!< количество сигналов на сетке X 
	int32_t & n_signal_idx_y,
	int32_t & shift_idx_y,
	double signal_idx_y[],
	int32_t fix_point_idx_x[],
	int32_t fix_point_idx_y[],
	int32_t fix_point_idx_r[],
	int32_t & max_shift_point_idx			//!< Радиус в индексах на сколько можно максимально сдвинуть при корреляции 
);

extern "C" EXPORT_FM int32_t FM_TestFunc(const int in, std::string& out);

extern "C" EXPORT_FM int32_t  FM_PrepareDataForCorrelationSeveral(
	const int32_t one_signal_size,			// число временных отсчетов одной трассы
	const int32_t n_trace,
	const double grid_init[],				// временные отсчеты [n_trace, one_signal_size]
	const double signal_init[],				// амплитуды для каждого значения времени [n_trace, one_signal_size]
	const int32_t n_fix_point_init,         //!< Количество зафиксированных точек, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const double fix_point_init[],			// зафиксированные точки для каждой из трасс [n_trace, one_signal_size]
	const double fix_point_init_r[],		// радиусы: их вот столько n_fix_point_init
	const double start_interval,
	const double finish_interval,
	const double step_grid,
	const double max_shift,
	int32_t & n_interval,
	const int32_t n_signal,				    //!< количество сигналов на каждой сетке
	int32_t & n_signal_idx,					//! [out] размер сетки (у всех трасс должен быть одинаковый) TODO: n.romanov, проверить, совпадает ли с one_signal_size
	double signal_idx[],					//! [out] [n_trace, one_signal_size]
	int32_t fix_point_idx[],				//! [out] фиксированные точки для всех трасс

	int32_t & shift_idx_x,
	double signal_idx_x[],
	const int32_t n_signal_y,					//!< количество сигналов на сетке X 
	int32_t & n_signal_idx_y,
	int32_t & shift_idx_y,
	double signal_idx_y[],
	int32_t fix_point_idx_r[],
	int32_t & max_shift_point_idx
);

//! @brief Функция оценивающая степень похожести одного участка сигнала на другой.
//! Оценивает похожесть двух точек.
typedef double(*MetricFunction)(
	const int32_t n_signal_x,
	const double signal_x[],
	const int32_t idx_x,
	const int32_t n_signal_y,
	const double signal_y[],
	const int32_t idx_y,
	const void* data
	);

typedef double(*MetricFunctionDouble)(
	const int16_t n_signal_x,
	const double signal_x[],
	const int16_t idx_x,
	const int16_t n_signal_y,
	const double signal_y[],
	const int16_t idx_y,
	const void* data
	);

typedef float(*MetricFunctionFloat)(
	const uint16_t n_signal_x,
	const float signal_x[],
	const uint16_t idx_x,
	const uint16_t n_signal_y,
	const float signal_y[],
	const uint16_t idx_y,
	const void* data
	);

template <typename FLOAT_T, typename INT_T>
using MetricFunctionTempl = FLOAT_T(*)(
	const INT_T n_signal_x,
	const FLOAT_T signal_x[],
	const INT_T idx_x,
	const INT_T n_signal_y,
	const FLOAT_T signal_y[],
	const INT_T idx_y,
	const void* data);

extern "C" EXPORT_FM double MetricFunctionSimpleConvolution(
	const int32_t n_signal_x,
	const double signal_x[],
	const int32_t idx_x,
	const int32_t n_signal_y,
	const double signal_y[],
	const int32_t idx_y,
	const void* data
);

/// <summary>
/// Индексы - тип int16_t
/// Значения - тип double
/// </summary>
/// <param name="n_signal_x"></param>
/// <param name="signal_x"></param>
/// <param name="idx_x"></param>
/// <param name="n_signal_y"></param>
/// <param name="signal_y"></param>
/// <param name="idx_y"></param>
/// <param name="data"></param>
/// <returns></returns>
extern "C" EXPORT_FM double MetricFunctionSimpleConvolutionDouble(
	const int16_t n_signal_x,
	const double signal_x[],
	const int16_t idx_x,
	const int16_t n_signal_y,
	const double signal_y[],
	const int16_t idx_y,
	const void* data
);

// uint16_t индексы
// float значения амплитуды
/*!
* @brief Волновой алгоритм.
*/
extern "C" EXPORT_FM int32_t  FM_CorrelationByWaveAlgorithm(
	const int32_t n_grid_x,				//!< количество сигналов на сетке X 
	const int32_t shift_idx_x,			//!< смещение индекса в сетке X до начала интервала
	const double signal_x[],			//!< сигналы на сетке X	[n_grid_x]
	const int32_t n_grid_y,				//!< количество сигналов на сетке Y 
	const int32_t shift_idx_y,			//!< смещение индекса
	const double signal_y[],			//!< сигналы на сетке Y	[n_grid_y]
	const int32_t n_interval,			//!< количество точек в коррелируемом интервале
	const int32_t max_shift_point_idx,	//!< Максимальный сдвиг каждой точки по индексам
	const int32_t n_fix_point,          //!< Количество зафиксированных точек, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const int32_t fix_point_idx_x[],	//!< 
	const int32_t fix_point_idx_y[],	//!< 
	const int32_t fix_point_idx_r[],    //!< для каждой точки задается окрестность в которой точку можно сдвинуть (для второй трассы (y))
	const MetricFunction fm,			//!< Функция метрики 
	const void* pdata_fm,				//!< Данные для функции метрики 
	double grid_y_by_x[],               //!< [out] значение индексов сетки Y по сетки X [n_interval]. Для каждой точки X выставлено значение по Y. Может быть отображение, выходящее за границу массива, это нормальное поведение.
	double& target_function				//!< [out] Значение целевой функции для маршрута. 
);

//! @brief Востановление по индексам значения функции с оконной фильтрацией и линейной интерполяцией
//! Обратное преобразование: индексы в значения.
extern "C" EXPORT_FM int32_t  FM_RecoverFunctionWithFilterWindowAndLineInterpolationByIndex(
	const int32_t n_grid_y,				//!< количество сигналов на сетке Y 
	const double signal_y[],			//!< сигналы на сетке Y	[n_grid_x]
	const int32_t n_interval,			//!< количесво элементов в grid_y_by_x 
	const double grid_y_by_x[],			//!< индексы в сигнал signal_y, возможно дробные индексы [n_interval]
	const int32_t r_idx_window,			//!< радиус оконного фильтра по индексам, если 0 фильтр отсутсвует
	double signal_y_by_grid_x[]			//!< значение функции [n_interval]
);


extern "C" EXPORT_FM int32_t  FM_Seismic2DCorrelationByWaveAlgorithm(
	const int32_t n_trace,				//!< количество трасс 
	const int32_t n_time,				//!< количество временных отсчётов
	const double signals[],			    //!< сигналы [n_trace,n_time]
	const int32_t max_shift_point_idx,	//!< Максимальный сдвиг в каждой точки по индексам между соседними трассами
	const int32_t n_fix_curves,         //!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const int32_t fix_point_idx[],		//!< -1 -> значит нет фиксации точки [n_trace, n_fix_curves]
	const int32_t fix_point_idx_r[],    //!< для каждой точки задается окрестность в которой точку можно сдвинуть [n_trace, n_fix_curves]
	const MetricFunction fm,			//!< Функция метрики 
	const void* pdata_fm,				//!< Данные для функции метрики 
	int32_t grid_y_by_x[]               //!< [out] Коды прослеженных пластов  [n_trace,n_time]
);

extern "C" EXPORT_FM int32_t  FM_Seismic3DTraceBorders(
	const int32_t n_trace_y,			//!< количество трасс по оси Y
	const int32_t n_trace_x,			//!< количество трасс по оси X
	const int32_t n_time,				//!< количество временных отсчётов
	const double  signals[],			//!< сигналы [n_trace_y,n_trace_x,n_time]
	const int32_t max_shift_point_idx,	//!< Максимальный сдвиг в каждой точки по индексам между соседними трассами
	const int32_t nborders,				//!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	int16_t borders_idx[],				//!< -1 -> значит нет фиксации точки [n_trace_y,n_trace_x, n_fix_curves]
	CallBackProgress callback,			//!< обратная связь
	int r_idx
);

//!Создаем корреляционный объект
extern "C" EXPORT_FM int32_t  FM_Seismic3DCorrelationByWaveCreateObject(
	const int32_t n_trace_y,			//!< количество трасс по оси Y
	const int32_t n_trace_x,			//!< количество трасс по оси X
	const int32_t n_time,				//!< количество временных отсчётов
	const double signals[],			    //!< сигналы [n_trace_y,n_trace_x,n_time]
	const int32_t max_shift_point_idx,	//!< Максимальный сдвиг в каждой точки по индексам между соседними трассами
	const int32_t n_fix_curves,         //!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const int16_t fix_point_idx[],		//!< -1 -> значит нет фиксации точки [n_trace_y,n_trace_x, n_fix_curves]
	const int16_t fix_point_idx_r[],    //!< для каждой точки задается окрестность в которой точку можно сдвинуть [n_trace_y,n_trace_x,n_fix_curves]
	CallBackProgress callback,			//!< обратная связь
	void**	obj_correlation				//!< [out] Коды прослеженных пластов  [n_trace,n_time]
);

extern "C" EXPORT_FM int32_t  FM_Seismic3DCorrelationByWaveTraceBorders(
	//const int32_t n_trace_x,			//!< количество трасс по оси X
	//const int32_t n_trace_y,			//!< количество трасс по оси Y
	//const int32_t n_time,				//!< количество временных отсчётов
	const void* obj_correlation,		//!< объект в котором сохранены  
	const int32_t idx_trace_x,			//!< индекс трассы с которой будем прослеживать границу
	const int32_t idx_trace_y,			//!< индекс трассы с которой будем прослеживать границу
	const int32_t n_points,				//!< количесво точек для прослеживания
	const int32_t point_idx[],			//!< индексы точек которые надо проследить [n_points]
	const int32_t surface_idx[]			//!< [out] поверхности [n_points,n_trace_y,n_trace_x]
);

extern "C" EXPORT_FM int32_t FM_Seismic3DCorrelationByWaveDeleteObject(void* obj_correlation);

extern "C" EXPORT_FM int32_t FM_Seismic3DCorrelationByWaveSizeObject(const void* obj_correlation,int64_t& size_object);

extern "C" EXPORT_FM int32_t FM_Seismic3DCorrelationByWaveSerializeObject(const void* obj_correlation, uint8_t* data);

extern "C" EXPORT_FM int32_t FM_Seismic3DCorrelationByWaveRestoreObject(void** obj_correlation, uint8_t * data);

//! Автоматическое определение периода
extern "C"  EXPORT_FM int32_t  FM_DefineOptimalRadius(
	const int32_t n_time,				//! число сигналов
	const double signals[],				//! сигналы 
	int32_t& idx_r						//! [out] оптимальный радиус
);

extern "C"  EXPORT_FM int32_t  FM_DefineOptimalRadius2D(
	const int32_t n_trace,				
	const int32_t n_time,
	const double signals[],
	int32_t & r_idx
);

extern "C" EXPORT_FM int32_t FM_CorrelationByWaveAlgorithmFor2TracesWithDefaultMetricFunction(
	//!< количество сигналов на сетке X
	const int16_t n_grid_x,
	//!< количество сигналов на сетке Y
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
);
