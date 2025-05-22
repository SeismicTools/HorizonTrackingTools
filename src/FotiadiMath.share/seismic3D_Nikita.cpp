#include "seismic3D_Nikita.h"
#include <unordered_map>
#include "DataToFillGraphPoints.h"
#include <Seismic3DGraph.h>
#include "S3GraphPrimSpanningTreeAdapter.h"
#include "CorrelationDataShare.h"
#include "ReflectingHorizonDefiner.h"
#include <ReflectingHorizonDefinerForEntireTrace.h>
#include "NotImplementedException.h"
#include <MainAndOtherPointsData.h>

int32_t FM_Seismic3DCorrelation(
	const int32_t inlineCount,
	const int32_t crosslineCount,
	const int32_t numOfSignalsAtOneTrace,
	const double signals[], // там идут сначала сигналы первой трассы Inline`а, потом все inline`ы, потом все crossline`s
	//!< Максимальный сдвиг в каждой точке по индексам между соседними трассами
	//!< Количество зафиксированных границ, значение по индексам X от Y с учетом shift_idx_y не должно превышать max_shift_point_idx
	const int32_t max_shift_point_idx,	
	const int32_t countOfFixedBorders,
	// массив, чтобы получить для каждой трассы id фиксированных точек отражающего горизонта
	int16_t surface_idx[],	//!< -1 -> значит нет фиксации точки [n_trace_y, n_trace_x, countOfFixedBorders]
	// TODO: n.romanov переделать для каждой точки свой радиус
	const int16_t fix_point_idx_r[],				// [countOfFixedBorders] 
	const void* callbackData,
	setPorgressValueFunc callback,
	int32_t r_idx,
	void** out_correlationData
)
{
	DataToFillGraphPoints dataToCreateGraph(inlineCount, 
		crosslineCount, numOfSignalsAtOneTrace, signals, max_shift_point_idx,
		countOfFixedBorders, surface_idx, fix_point_idx_r, r_idx, callback, callbackData);
	Seismic3DGraph seismic3DGraph(dataToCreateGraph);

	if(callback(0, callbackData))
	{
		return FM_CALCULATIONS_STOPPED_BY_UI;
	}
	
	auto s3DCorrelationEdgesToStore = S3GraphPrimSpanningTreeAdapter::GetS3DEdgesAtSpanningTree(seismic3DGraph);

	auto allCoordsOfPoints = seismic3DGraph.GetAllExistingTraceCoordinates();
	auto dataToShare = new CorrelationDataShare(*allCoordsOfPoints, 
		*s3DCorrelationEdgesToStore,
		seismic3DGraph.GetIlCount(),
		seismic3DGraph.GetClCount());
	*out_correlationData = dataToShare;

	delete allCoordsOfPoints;
	delete s3DCorrelationEdgesToStore;
	return FM_ERROR_NO;
}

EXPORT_FM int32_t FM_Seismic3DCorrelation_one_to_many(
	const int32_t inlineCount, 
	const int32_t crosslineCount, 
	const int32_t numOfSignalsAtOneTrace, 
	const double signals[], 
	const int32_t max_shift_point_idx, 
	const int32_t countOfFixedBorders, 
	int16_t surface_idx[], 
	const int16_t fix_point_idx_r[], 
	const void* callbackData, 
	setPorgressValueFunc callback, 
	int32_t r_idx, 
	const int32_t mainTraceCoordByX, 
	const int32_t mainTraceCoordByY, 
	const int32_t horizonsBreadth,			// ширина границ
	const int32_t spaceBetweenHorizons,		// расстояние между границами
	void** out_correlationData)
{
	try
	{
		MainAndOtherPointsData data(inlineCount,
			crosslineCount,
			numOfSignalsAtOneTrace,
			signals,
			max_shift_point_idx,
			countOfFixedBorders,
			surface_idx,
			fix_point_idx_r,
			callbackData,
			callback,
			r_idx,
			mainTraceCoordByX,
			mainTraceCoordByY,
			horizonsBreadth,
			spaceBetweenHorizons);

		data.CorrelateMainTraceWithAll();
		if (callback(0, callbackData))
		{
			return FM_CALCULATIONS_STOPPED_BY_UI;
		}
		*out_correlationData = data.GetHorizonsDataOut();
	}
	catch(const FotiadiMath_Exceptions::ArgumentIllegalException& e)
	{
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}
	
	return FM_ERROR_NO;
}

EXPORT_FM int32_t FM_Seismic3DCorrelation_one_to_many_with_different_trace_size(
	const int32_t inlineCount, 
	const int32_t crosslineCount, 
	const int32_t numOfSignalsAtOneTrace, 
	const double signals[], 
	const int32_t max_shift_point_idx, 
	const int32_t countOfFixedBorders, 
	int16_t surface_idx[], 
	const int16_t fix_point_idx_r[], 
	const void* callbackData, 
	setPorgressValueFunc callback, 
	int32_t r_idx, 
	const int32_t mainTraceCoordByX, 
	const int32_t mainTraceCoordByY, 
	void** out_correlationData)
{
	throw new FotiadiMath_Exceptions::NotImplementedException();
}

int32_t FM_S3DDefineReflectingHorizons(
	const void* savedObjectWithCorrelationData,
	const int32_t traceCoordByX,
	const int32_t traceCoordByY,
	const int32_t numOfPointsToFindReflectingHorizon,
	const int32_t idOfPointsAtTraceToFindReflectingHorizon[],
	//!< [out] поверхности [numOfPointsToFindReflectingHorizon,n_trace_y,n_trace_x].
	//! для каждой точки своя поверхность
	//! одна поверхность = id`шки в каждой трассе куба
	int32_t out_surfaceIds[]
)
{
	const CorrelationDataShare* correlationData = static_cast<
		const CorrelationDataShare*>(savedObjectWithCorrelationData);
	
	CoordinatesInCube coords(traceCoordByY, traceCoordByX);
	if(!correlationData->isContainPoint(coords))
	{
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}

	for(int i = 0, j = 0; i < numOfPointsToFindReflectingHorizon; i++)
	{
		auto currentPointId = idOfPointsAtTraceToFindReflectingHorizon[i];

		auto horizonIds = ReflectingHorizonDefiner::FindReflectingHorizonsForEntireTrace(*correlationData, coords, currentPointId);

		for(int k = 0; k < horizonIds->size(); k++)
		{
			out_surfaceIds[j] = horizonIds->operator[](k);
			j++;
		}

		delete horizonIds;
	}

	return FM_ERROR_NO;
}

EXPORT_FM int32_t FM_S3DDefineReflectingHorizonsForEntireTrace(
	const void* savedObjectWithCorrelationData, 
	const int32_t traceCoordByX, 
	const int32_t traceCoordByY, 
	int32_t** out_surfaceIds)
{
	const CorrelationDataShare* correlationData = static_cast<
		const CorrelationDataShare*>(savedObjectWithCorrelationData);

	CoordinatesInCube coords(traceCoordByY, traceCoordByX);
	if (!correlationData->isContainPoint(coords))
	{
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}

	(*out_surfaceIds) = ReflectingHorizonDefinerForEntireTrace::FindReflectingHorizonsForEntireTrace(
		traceCoordByX, 
		traceCoordByY,
		*correlationData);

	return FM_ERROR_NO;
}

EXPORT_FM int32_t FM_S3DDefineReflectingHorizonsForEntireTraceNew(
	const void* savedObjectWithCorrelationData, 
	const int32_t inlineId, // inline опорной трассы
	const int32_t crosslineId, // crossline опорной трассы
	const int32_t countOfFixedBorders, 
	// здесь именно countOfFixedBorders чисел. Это 100%. Тоесть все фиксированные индексы для опорной трассы
	// здесь не глобальные id, а относительно minGlobalId
	int16_t surface_idx[], 
	const int32_t spacesBetweenTwoHorizons, 
	const int32_t sizeOfOneHorizon, 
	const void* callbackData, // тут будет указатель на объект класса PluginProgressReporterInterface
	setPorgressValueFunc callback,
	const int32_t horizonsBreadth,			// ширина границ
	int32_t** out_surfaceIds
	)
{
	const CorrelationDataShare* correlationData = static_cast<
		const CorrelationDataShare*>(savedObjectWithCorrelationData);


//#ifdef _DEBUG
	// сохраняет остовное дерево в пару файлов
	// points.csv - вершины
	// edges.csv - ребра
	correlationData->SaveGraphToCsv();
	correlationData->SaveDepthOfTreeToFile(inlineId, crosslineId);
//#endif

	CoordinatesInCube coords(inlineId, crosslineId);
	if (!correlationData->isContainPoint(coords))
	{
		return FM_ERROR_PARAMETRS_NOT_VALID;
	}
	// TODO: n.romanov прогресс callback
	try
	{
		(*out_surfaceIds) = ReflectingHorizonDefinerForEntireTrace::FindReflectingHorizonsForEntireTrace(
			inlineId,
			crosslineId,
			*correlationData,
			countOfFixedBorders,
			surface_idx,
			spacesBetweenTwoHorizons,
			sizeOfOneHorizon,
			callbackData,
			callback,
			horizonsBreadth);
	}
	catch(const std::exception& e)
	{
		return FM_CALCULATIONS_STOPPED_BY_UI;
	}

	return FM_ERROR_NO;
}

EXPORT_FM int32_t FM_S3D_FreeArray(const void* savedObjectWithCorrelationData)
{
	delete[] savedObjectWithCorrelationData;

	return FM_ERROR_NO;
}

EXPORT_FM int32_t FM_S3D_Free(const void* savedObject)
{
	delete savedObject;

	return FM_ERROR_NO;
}

EXPORT_FM int32_t FM_S3D_FindReflectingHorizonsForOneTrace(const int32_t inlineCount, const int32_t crosslineCount, const int32_t numOfSignalsAtOneTrace, const double signals[], const int32_t coordX, const int32_t coordY, const int32_t max_shift_point_idx, const int32_t countOfFixedBorders, const int16_t surface_idx[], const int16_t fix_point_idx_r[], CallBackProgress callback, int32_t r_idx, int32_t** out_surfaceIds)
{
	return FM_ERROR_NO;
}
