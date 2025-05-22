#pragma once

#include <Seismic3DPointWithNeighboursAndSimilarityValues.h>
#include <fotiadi_math_private.h>

// TODO: n.romanov ВОПРОС новый проект с тестами не получилось собрать FotiadiMathShare.test
class Point3DSimilarityHelper {
    public:
        Point3DSimilarityHelper(const Seismic3DPointWithNeighboursAndSimilarityValues& point1,
            const Seismic3DPointWithNeighboursAndSimilarityValues& point2,
            const int32_t maxShiftAtEachPointByIndexBetweenNeighboringTraces,
            const int32_t countOfFixedBorders, 
            const int16_t* idsOfFixedPointsAtXTrace,
            const int16_t* idsOfFixedPointsAtYTrace,
            const int16_t* fix_point_idx_r,
            const int32_t& dataForMetricFunction);

        ~Point3DSimilarityHelper();
    
        double getSimilarityOfTwoPoints();

        double getSimilarityOfTwoPointsNotEqualsLenghtOfTraces();

        const std::vector<int16_t>& GetMappingFirstTraceOnSecond() const;
        const std::vector<int16_t>& GetMappingSecondTraceOnSFirst() const;
    
    private:
        // радиус окрестности, в которой будет проверяться похожесть двух точек, должна быть ~пол периода (не больше размера массива)
        const Seismic3DPointWithNeighboursAndSimilarityValues& _point1;
        const Seismic3DPointWithNeighboursAndSimilarityValues& _point2;
        const int32_t _maxShiftAtEachPointByIndexBetweenNeighboringTraces;
        const int16_t _countOfFixedBorders;
        const int16_t* _idsOfFixedPointsAtXTrace;
        const int16_t* _idsOfFixedPointsAtYTrace;
        const int16_t* _fix_point_idx_r;
        std::vector<int16_t>* _mappingXOnYTrace;
        std::vector<int16_t>* _mappingYOnXTrace;
        const DataForMetricConvolutionWithShift _data_for_metric;
};
