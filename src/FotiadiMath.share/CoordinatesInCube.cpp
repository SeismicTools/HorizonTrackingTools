#include "CoordinatesInCube.h"

// сюда надо передавать crossline count
const int32_t CoordinatesInCube::GetIdAtOneDimensionalArray(const int32_t& numOfElemAtOneRaw) const
{
	return inlineId * numOfElemAtOneRaw + crosslineId;
}
