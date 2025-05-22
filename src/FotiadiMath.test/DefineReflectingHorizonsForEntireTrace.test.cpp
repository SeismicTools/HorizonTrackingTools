#include <gtest/gtest.h>
#include "Helper.h"
#include <seismic3D_Nikita.h>
#include <HorizonWrapper.h>

namespace define_reflecting_horizons_for_entire_trace
{
	static TEST(DefineReflectingHorizons, Test1) {
		// given CorrelationDataShare with 4 points
		auto corDataShare = Helper::GetCorrelationDataShareObject();
		void* classPointer = corDataShare;
		CoordinatesInCube traceCoords(1, 0);

		int32_t* out_surfaceIds = nullptr;

		// when пытаемся получить отражающие горизонты для выбранной трассы
		auto errorCode = FM_S3DDefineReflectingHorizonsForEntireTrace(classPointer,
			traceCoords.crosslineId,
			traceCoords.inlineId,
			&out_surfaceIds);

		// then
		// получим всего 3 границы, т.к. в трассе 3 значения
		// размер куба 2*2, значит размер out_surfaceIds = 2*2*3
		int32_t borderNum = 0; // точка 00
		const int32_t toSkip = 2*2;
		EXPECT_EQ(out_surfaceIds[0 * toSkip + borderNum], HorizonWrapper::NothingValue);
		EXPECT_EQ(out_surfaceIds[1 * toSkip + borderNum], 1);
		EXPECT_EQ(out_surfaceIds[2 * toSkip + borderNum], -2);

		borderNum = 1; // точка 01
		EXPECT_EQ(out_surfaceIds[0 * toSkip + borderNum], 1);
		EXPECT_EQ(out_surfaceIds[1 * toSkip + borderNum], -2);
		EXPECT_EQ(out_surfaceIds[2 * toSkip + borderNum], HorizonWrapper::NothingValue);

		borderNum = 2; // точка 10  --- стартовая точка
		EXPECT_EQ(out_surfaceIds[0 * toSkip + borderNum], 1);
		EXPECT_EQ(out_surfaceIds[1 * toSkip + borderNum], -2);
		EXPECT_EQ(out_surfaceIds[2 * toSkip + borderNum], 3);

		borderNum = 3; // точка 11
		EXPECT_EQ(out_surfaceIds[0 * toSkip + borderNum], -2);
		EXPECT_EQ(out_surfaceIds[1 * toSkip + borderNum], 3);
		EXPECT_EQ(out_surfaceIds[2 * toSkip + borderNum], HorizonWrapper::NothingValue);

		FM_S3D_FreeArray(out_surfaceIds);
		delete corDataShare;
	}
}
