#pragma once

#include <vector>

struct CoordinatesInCube {
    int32_t inlineId;
    int32_t crosslineId;

    CoordinatesInCube(int32_t inlineId, int32_t crosslineId)
        : inlineId(inlineId), crosslineId(crosslineId)
    {

    }

    bool operator==(const CoordinatesInCube& other) const {
        return inlineId == other.inlineId
            && crosslineId == other.crosslineId;
    }

    const int32_t GetIdAtOneDimensionalArray(const int32_t& numOfElemAtOneRaw) const;
};

namespace std {
    template <>
    struct hash<CoordinatesInCube> {
        size_t operator()(const CoordinatesInCube& coord) const {
            return std::hash<int>()(coord.inlineId) ^ std::hash<int>()(coord.crosslineId);
        }
    };
}
