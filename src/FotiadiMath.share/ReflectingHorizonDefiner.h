#pragma once

#include "CorrelationDataShare.h"

class ReflectingHorizonDefiner
{
	public:
		static std::vector<int32_t>* FindReflectingHorizonsForEntireTrace(const CorrelationDataShare& corrData,
			const CoordinatesInCube& traceCoords,
			const int32_t& idOfPointAtTrace);

	private:

};
