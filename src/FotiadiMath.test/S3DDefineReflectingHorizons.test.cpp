#include <gtest/gtest.h>
#include "seismic3D_Nikita.h"
#include <CorrelationDataShare.cpp>
#include "S3DEdge.cpp"
#include "Helper.h"
#include "CoordinatesInCube.cpp"

namespace seismic_3d_correlation_tests
{
	static TEST(DefineReflectingHorizons, TryingToBuildReflectionHorizonForNotExistingPoint) {
		// given CorrelationDataShare без точки куба, для который мы хотим получить отражающий горизонт
		std::vector<CoordinatesInCube> allCoordsOfPoints;
		std::vector<S3DEdge> graphEdges;
		const int32_t sizeOfCubeByX = 50;
		const int32_t sizeOfCubeByY = 50;
		CorrelationDataShare corDataShare(allCoordsOfPoints, graphEdges, sizeOfCubeByY, sizeOfCubeByX);
		void* classPointer = &corDataShare;
		CoordinatesInCube traceCoords(5, 6);
		auto numOfPointsToFindReflectionHorizons = 1;
		int32_t idOfPointsAtTraceToFindReflectingHorizon[] = {546};
		int32_t out_surfaceIds[sizeOfCubeByX * sizeOfCubeByY];

		// when пытаемся получить горизонт
		auto errorCode = FM_S3DDefineReflectingHorizons(classPointer, 
			traceCoords.crosslineId,
			traceCoords.inlineId,
			numOfPointsToFindReflectionHorizons,
			idOfPointsAtTraceToFindReflectingHorizon,
			out_surfaceIds);

		// then
		EXPECT_EQ(errorCode, FM_ERROR_PARAMETRS_NOT_VALID);
	}

	static TEST(DefineReflectingHorizons, UsualTest)
	{
		// given CorrelationDataShare with 4 points
		auto corDataShare = Helper::GetCorrelationDataShareObject();
		void* classPointer = corDataShare;
		CoordinatesInCube traceCoords(1, 0);
		const auto numOfPointsToFindReflectionHorizons = 1;
		int32_t idOfPointsAtTraceToFindReflectingHorizon[] = { 1 };
		int32_t out_surfaceIds[Helper::SizeOfCubeByX 
			* Helper::SizeOfCubeByY 
			* numOfPointsToFindReflectionHorizons];

		// when пытаемся получить горизонт
		auto errorCode = FM_S3DDefineReflectingHorizons(classPointer,
			traceCoords.crosslineId,
			traceCoords.inlineId,
			numOfPointsToFindReflectionHorizons,
			idOfPointsAtTraceToFindReflectingHorizon,
			out_surfaceIds);

		// then
		EXPECT_EQ(out_surfaceIds[0], 2);
		EXPECT_EQ(out_surfaceIds[1], 1);
		EXPECT_EQ(out_surfaceIds[2], 1); // изначально искомый id
		EXPECT_EQ(out_surfaceIds[3], 0);

		delete corDataShare;
	}
}
