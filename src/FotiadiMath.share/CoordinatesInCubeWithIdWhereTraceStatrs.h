#pragma once

#include <vector>

struct CoordinatesInCubeWithIdWhereTraceStatrs {
    int32_t inlineId;
    int32_t crosslineId;
    int32_t idWhereTraceStarts; // все отображания трассы на трассу сохранены без этого смещения

    CoordinatesInCubeWithIdWhereTraceStatrs(int32_t inlineId, int32_t crosslineId)
        : inlineId(inlineId), crosslineId(crosslineId), idWhereTraceStarts(0)
    {

    }

    CoordinatesInCubeWithIdWhereTraceStatrs(int32_t inlineId, int32_t crosslineId, int32_t idWhereTraceStarts)
        : inlineId(inlineId), crosslineId(crosslineId), idWhereTraceStarts(idWhereTraceStarts)
    {

    }

    bool operator==(const CoordinatesInCubeWithIdWhereTraceStatrs& other) const {
        return inlineId == other.inlineId
            && crosslineId == other.crosslineId;
    }

    const int32_t GetIdAtOneDimensionalArray(const int32_t& numOfElemAtOneRaw) const
    {
        return inlineId * numOfElemAtOneRaw + crosslineId;
    }
};

namespace std {
    template <>
    struct hash<CoordinatesInCubeWithIdWhereTraceStatrs> {
        size_t operator()(const CoordinatesInCubeWithIdWhereTraceStatrs& coord) const {
            return std::hash<int>()(coord.inlineId) ^ std::hash<int>()(coord.crosslineId);
        }
    };
}
