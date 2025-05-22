#pragma once

#include "CoordinatesInCube.h"

struct CoordinatesAtCube3d
{
	CoordinatesInCube coords2d;
	int16_t depth;

	CoordinatesAtCube3d(CoordinatesInCube coords2d, int16_t depth)
		: coords2d(coords2d), depth(depth)
	{

	}
};
