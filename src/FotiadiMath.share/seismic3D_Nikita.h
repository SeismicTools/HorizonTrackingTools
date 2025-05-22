#pragma once

#include "fotiadi_math.h"
#include "Seismic3DPointWithNeighboursAndSimilarityValues.h"
#include "TypedefStuff.h"

extern "C" EXPORT_FM int32_t FM_Seismic3DCorrelation(
	const int32_t inlineCount,
	const int32_t crosslineCount,
	const int32_t numOfSignalsAtOneTrace,
	const double signals[],
	const int32_t max_shift_point_idx,
	const int32_t countOfFixedBorders,
	int16_t surface_idx[],
	const int16_t fix_point_idx_r[], // по умолчанию 0
	const void* callbackData, // тут будет указатель на объект класса PluginProgressReporterInterface
	setPorgressValueFunc callback,
	int32_t r_idx,
	void** out_correlationData
);

// подход, использованный в W - SEIS для корреляции с опорной трассой
// корреляция одной, опорной трассы ко всем остальным. 
extern "C" EXPORT_FM int32_t FM_Seismic3DCorrelation_one_to_many(
	const int32_t inlineCount,
	const int32_t crosslineCount,
	const int32_t numOfSignalsAtOneTrace,
	const double allSignals[],				// изначально все равного размера, но под капотом будут обрезаться для построения корреляции
	const int32_t max_shift_point_idx,		// размер диагонали в матрице поиска пути (если 0 значит матрицу заполняем полностью)
	const int32_t countOfFixedBorders,		// >= 2 и все точки определены
	int16_t surface_idx[], 					// в опорной трассе все границы определены + самая верхняя и нижняя границы полностью определены
	const int16_t fix_point_idx_r[],		// по умолчанию 0
	const void* callbackData, 				// тут будет указатель на объект класса PluginProgressReporterInterface
	setPorgressValueFunc callback,
	int32_t r_idx,							// Радиус окно в котором проводится сравнение 
	const int32_t mainTraceCoordByX,		// координата x опорной трассы (inline)
	const int32_t mainTraceCoordByY,		// координата y опорной трассы (crossline)
	const int32_t horizonsBreadth,			// ширина границ
	const int32_t spaceBetweenHorizons,		// расстояние между границами
	void** out_correlationData
);

// подход, использованный в W-SEIS для корреляции с соседями
extern "C" EXPORT_FM int32_t FM_S3DDefineReflectingHorizonsForEntireTraceNew(
	const void* savedObjectWithCorrelationData,
	const int32_t traceCoordByX,
	const int32_t traceCoordByY,
	const int32_t countOfFixedBorders, // число отражающих горизонтов
	int16_t surface_idx[], // размер = countOfFixedBorders. Здесь лежат id, которые зафиксированы
	const int32_t spacesBetweenTwoHorizons, // растояние в индексах между двумя границами
	const int32_t sizeOfOneHorizon, // НЕ ИСПОЛЬЗУЕТСЯ, вместо неё используется horizonsBreadth
	const void* callbackData, // тут будет указатель на объект класса PluginProgressReporterInterface
	setPorgressValueFunc callback,
	const int32_t horizonsBreadth,			// ширина границ в индексах
	int32_t** out_surfaceIds
);

extern "C" EXPORT_FM int32_t FM_S3D_FreeArray(
	const void* savedObjectWithCorrelationData
);

extern "C" EXPORT_FM int32_t FM_S3D_Free(
	const void* savedObject
);

/// <summary>
/// Устарел. Нет реализации.
/// 1) Получает отображение из трассы с координатами (coordX, coordY) во все остальные
/// 2) Каждой точке выбранной трассы присваивается номер отражающего горизонта
/// 3) Номер протягивается в каждую точку, куда он отображается
/// 4) На выходе получаем куб с номерами отражающих горизонтов, исходящих из одной выбранной трассы
/// </summary>
/// <returns></returns>
extern "C" EXPORT_FM int32_t FM_S3D_FindReflectingHorizonsForOneTrace(
	const int32_t inlineCount,
	const int32_t crosslineCount,
	const int32_t numOfSignalsAtOneTrace, // или глубина везде разная? (массив с размерами)
	const double signals[],
	const int32_t coordX,
	const int32_t coordY,
	const int32_t max_shift_point_idx,
	const int32_t countOfFixedBorders, // всегда 2+ границы задается
	// массив, чтобы получить для каждой трассы id фиксированных точек отражающего горизонта
	const int16_t surface_idx[], // если есть null`ы, то ошибка
	const int16_t fix_point_idx_r[], // для каждой фиксированной точки - свой радиус фиксации?
	CallBackProgress callback, // обратная связь
	int32_t r_idx, // данные для функции метрики
	int32_t** out_surfaceIds
);

/// <summary>
/// Устарел. Первый подход, который делал. Уже не используется
/// Заполняет куб номерами границ.
/// Число границ = число отсчетов в одной трассе.
/// Номера границ хранятся по отсчетам, то-бишь уровнями отсчетов.
/// Например, если имеем куб 2*2*3, где 2 - размер по x, 2 - размер по y, 3 - число отсчетов, то:
/// первые 4 числа - это номера границ для отсчета 0 на 4 трассах
/// слеудющие 4 числа - это номера границ для отсчета 1 на 4 трассах.
/// Другой вариант (ТАК МОЖНО СДЕЛАТЬ, НО ЗДЕСЬ НЕ ТАК) - хранить данные пачкой границ отсчетов одной трассы:
/// 3 числа - первая трасса, следующие 3 - вторая трасса и т.д.
/// </summary>
/// <param name="savedObjectWithCorrelationData"></param>
/// <param name="traceCoordByX"></param>
/// <param name="traceCoordByY"></param>
/// <param name="out_surfaceIds">Значение std::numeric_limits<int32_t>::min() в отсчете означает, что граница не определена.</param>
/// <returns></returns>
extern "C" EXPORT_FM int32_t FM_S3DDefineReflectingHorizonsForEntireTrace(
	const void* savedObjectWithCorrelationData,
	const int32_t traceCoordByX,
	const int32_t traceCoordByY,
	int32_t** out_surfaceIds
);

// старое. Вроде нигде не используется
extern "C" EXPORT_FM int32_t FM_S3DDefineReflectingHorizons(
	const void* savedObjectWithCorrelationData,
	const int32_t traceCoordByX,
	const int32_t traceCoordByY,
	const int32_t numOfPointsToFindReflectingHorizon,
	const int32_t idOfPointsAtTraceToFindReflectingHorizon[],
	//!< [out] поверхности [numOfPointsToFindReflectingHorizon,n_trace_y,n_trace_x].
	//! для каждой точки своя поверхность
	//! одна поверхность = id`шки в каждой трассе куба
	int32_t out_surfaceIds[]
);
