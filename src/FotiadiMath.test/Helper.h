#pragma once

#include <CorrelationDataShare.h>

class Helper
{
	public:
		static CorrelationDataShare* GetCorrelationDataShareObject();

		static const int32_t SizeOfCubeByX = 2;
		static const int32_t SizeOfCubeByY = 2;

};
