#include "Point3DSimilarityHelper.h"
#include "correlation_trace_new.h"

Point3DSimilarityHelper::Point3DSimilarityHelper(const Seismic3DPointWithNeighboursAndSimilarityValues& point1,
    const Seismic3DPointWithNeighboursAndSimilarityValues& point2,
    const int32_t maxShiftAtEachPointByIndexBetweenNeighboringTraces,
    const int32_t countOfFixedBorders,
    const int16_t* idsOfFixedPointsAtXTrace,
    const int16_t* idsOfFixedPointsAtYTrace,
    const int16_t* fix_point_idx_r,
    const int32_t& dataForMetricFunction)
        : _point1(point1), _point2(point2),
            _maxShiftAtEachPointByIndexBetweenNeighboringTraces(maxShiftAtEachPointByIndexBetweenNeighboringTraces),
            _countOfFixedBorders(countOfFixedBorders),
            _idsOfFixedPointsAtXTrace(idsOfFixedPointsAtXTrace),
            _idsOfFixedPointsAtYTrace(idsOfFixedPointsAtYTrace),
            _fix_point_idx_r(fix_point_idx_r),
            _mappingXOnYTrace(nullptr),
            _mappingYOnXTrace(nullptr),
            _data_for_metric{(uint16_t)dataForMetricFunction, 2.0 / 3, 0, 0} // TODO: n.romanov ВОПРОС что передавать (данные для ф-ии метрики)
{

}

Point3DSimilarityHelper::~Point3DSimilarityHelper()
{
    delete _mappingXOnYTrace;
    _mappingXOnYTrace = nullptr;
    delete _mappingYOnXTrace;
    _mappingYOnXTrace = nullptr;
}

double Point3DSimilarityHelper::getSimilarityOfTwoPoints()
{
    auto numOfSignalsForEachOfTwoTraces = _point1.GetNumOfSignals();
    auto pointFirstSignals = _point1.GetSignals();
    auto pointSecondSignals = _point2.GetSignals();
    _mappingXOnYTrace = new std::vector<int16_t>(numOfSignalsForEachOfTwoTraces);
    _mappingYOnXTrace = new std::vector<int16_t>(numOfSignalsForEachOfTwoTraces);

    double similarityOfTraces;

    FM_CorrelationByWaveAlgorithmForTrace(
        numOfSignalsForEachOfTwoTraces,
        pointFirstSignals, 
        pointSecondSignals,
        _maxShiftAtEachPointByIndexBetweenNeighboringTraces,
        _countOfFixedBorders,
        _idsOfFixedPointsAtXTrace, 
        _idsOfFixedPointsAtYTrace, 
        _fix_point_idx_r,
        MetricFunctionSimpleConvolutionDouble,
        (const void*)&_data_for_metric,
        _mappingXOnYTrace->data(),
        _mappingYOnXTrace->data(),
        similarityOfTraces);

    return similarityOfTraces;
}

double Point3DSimilarityHelper::getSimilarityOfTwoPointsNotEqualsLenghtOfTraces()
{
    auto numOfSignalsForFirstTrace = _point1.GetNumOfSignals();
    auto numOfSignalsForSecondTrace = _point2.GetNumOfSignals();
    auto pointFirstSignals = _point1.GetSignals();
    auto pointSecondSignals = _point2.GetSignals();
    _mappingXOnYTrace = new std::vector<int16_t>(numOfSignalsForFirstTrace);
    _mappingYOnXTrace = new std::vector<int16_t>(numOfSignalsForSecondTrace);

    double similarityOfTraces;

    diff_trace_sizes::FM_CorrelationByWaveAlgorithmForTrace(
        numOfSignalsForFirstTrace,
        numOfSignalsForSecondTrace,
        pointFirstSignals,
        pointSecondSignals,
        _maxShiftAtEachPointByIndexBetweenNeighboringTraces,
        _countOfFixedBorders,
        _idsOfFixedPointsAtXTrace,
        _idsOfFixedPointsAtYTrace,
        _fix_point_idx_r,
        MetricFunctionSimpleConvolutionDouble,
        (const void*)&_data_for_metric,
        _mappingXOnYTrace->data(),
        _mappingYOnXTrace->data(),
        similarityOfTraces
    );

    return similarityOfTraces;
}

const std::vector<int16_t>& Point3DSimilarityHelper::GetMappingFirstTraceOnSecond() const
{
    return *_mappingXOnYTrace;
}

const std::vector<int16_t>& Point3DSimilarityHelper::GetMappingSecondTraceOnSFirst() const
{
    return *_mappingYOnXTrace;
}