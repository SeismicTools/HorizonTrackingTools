#include <gtest/gtest.h>
#include "seismic3D_Nikita.h"
#include "Seismic3DCube.cpp"
#include <chrono>
#include <functional>

namespace seismic_3d_correlation_tests
{
	static const std::string с_baseTestsPath = "../../FotiadiMath.test/Data/seismic3D_Nikita/";

	static const Seismic3DCube* TryToGetSeismic3DCubeFromFile(std::string);
	static int32_t ExecLambdaFuncAndMeasureExecTimeAndLogToConsoleAndGetErrorCode(std::function<int32_t()>);

	static TEST(ObjectCreation, Ex1) {

		// given 4 traces with 3 signal values
		int32_t n_trace_y = 2;
		int32_t n_trace_x = 2;
		int32_t n_time = 3;
		double signals[] = { 1,-2,2, 1,-2,2,
							 -2,5,-3, -2,5,-3 };
		const int16_t maxShiftAtEachPointByIndexBetweenNeighboringTraces = 15;
		void* correlationDataObject = nullptr;
		int32_t r_idx = 0; // TODO: n.romanov ВОПРОС что передавать. Должен быть равен длине волны

		// when
		auto errorCode = FM_Seismic3DCorrelation(n_trace_y, 
			n_trace_x, 
			n_time, signals, 
			maxShiftAtEachPointByIndexBetweenNeighboringTraces,
			0, NULL, NULL, NULL, NULL, r_idx, &correlationDataObject);

		// then
		EXPECT_EQ(errorCode, 0);

		int32_t* out_surfaceIds = nullptr;
		errorCode = FM_S3DDefineReflectingHorizonsForEntireTrace(correlationDataObject, 0, 0, &out_surfaceIds);
		FM_S3D_FreeArray(out_surfaceIds);

		delete correlationDataObject;
	}

	static TEST(ObjectCreation, Ex2) {
		auto nameOfCubeFileForThisTest = std::string("ex2_cube.bin");
		try
		{
			// given
			auto fullPathToCube = с_baseTestsPath + nameOfCubeFileForThisTest;
			const auto seismic3DCube = TryToGetSeismic3DCubeFromFile(fullPathToCube);
			const int16_t maxShiftAtEachPointByIndexBetweenNeighboringTraces = 15;
			void* correlationDataObject = nullptr;
			int32_t r_idx = 0; // TODO: n.romanov ВОПРОС что передавать. Должен быть равен длине волны

			// when
			auto errorCode = ExecLambdaFuncAndMeasureExecTimeAndLogToConsoleAndGetErrorCode(
				[&seismic3DCube, &correlationDataObject, &r_idx]() -> int32_t {
					return FM_Seismic3DCorrelation(seismic3DCube->TryToGetInlineCount(),
						seismic3DCube->TryToGetCrosslineCount(),
						seismic3DCube->TryToGetNumOfSignalsAtOneTrace(),
						seismic3DCube->TryToGetAllCubeData(),
						maxShiftAtEachPointByIndexBetweenNeighboringTraces,
						0, NULL, NULL, NULL, NULL, r_idx, &correlationDataObject);
				}
			);

			delete seismic3DCube;
			// then
			EXPECT_EQ(errorCode, 0);
			delete correlationDataObject;
		}
		catch(const std::exception& ex)
		{
			throw;
		}
	}

	static TEST(ObjectCreation, Ex3) {

		// given 4 traces with 3 signal values
		int32_t n_trace_y = 2;
		int32_t n_trace_x = 2;
		int32_t n_time = 3;
		double signals[] = { 1,-2,2, 1,-2,2,
							 -2,5,-3, -2,5,-3 };
		const int16_t maxShiftAtEachPointByIndexBetweenNeighboringTraces = 15;
		const int32_t countOfFixedBorders = 1;
		int16_t surface_idx[] = { 1, 1, 0, 0 }; // id фиксированных точек в каждой из трасс
		const int16_t fix_point_idx_r[] = { 2 };
		void* correlationDataObject = nullptr;
		int32_t r_idx = 0; // TODO: n.romanov ВОПРОС что передавать. Должен быть равен длине волны

		// when
		auto errorCode = FM_Seismic3DCorrelation(n_trace_y,
			n_trace_x,
			n_time, signals,
			maxShiftAtEachPointByIndexBetweenNeighboringTraces,
			0, surface_idx, fix_point_idx_r, NULL, NULL, r_idx, &correlationDataObject);

		// then
		EXPECT_EQ(errorCode, 0);
		delete correlationDataObject;
	}

	static TEST(ObjectCreation, Ex4) 
	{
		int32_t n_trace_y = 3;
		int32_t n_trace_x = 2;
		int32_t n_time = 3;
		double signals[] = { 1,-2,2, 1,-2,2, 1,-2,2
							 -2,5,-3, -2,5,-3, -2,5,-3 };
		const int16_t maxShiftAtEachPointByIndexBetweenNeighboringTraces = 15;
		const int32_t countOfFixedBorders = 1;
		int16_t surface_idx[] = { 1, 1, 0, 0, 1, 1 }; // id фиксированных точек в каждой из трасс
		const int16_t fix_point_idx_r[] = { 2 };
		void* correlationDataObject = nullptr;
		int32_t r_idx = 0; // TODO: n.romanov ВОПРОС что передавать. Должен быть равен длине волны

		// when
		auto errorCode = FM_Seismic3DCorrelation(n_trace_y,
			n_trace_x,
			n_time, signals,
			maxShiftAtEachPointByIndexBetweenNeighboringTraces,
			0, surface_idx, fix_point_idx_r, NULL, NULL, r_idx, &correlationDataObject);

		// then
		EXPECT_EQ(errorCode, 0);
		delete correlationDataObject;
	}

	static const Seismic3DCube* TryToGetSeismic3DCubeFromFile(std::string fullPathToCube)
	{
		std::ifstream file(fullPathToCube, std::ios::binary);
		if (!file.is_open()) {
			auto errorText
				= std::string("Error occured when trying to load cube data. FullPathToCube = ")
				+ fullPathToCube;
			throw std::runtime_error(errorText);
		}

		auto seismic3DCube = Seismic3DCube::CreateUninitializedInstance();
		file >> *seismic3DCube;
		file.close();

		return seismic3DCube;
	}

	static int32_t ExecLambdaFuncAndMeasureExecTimeAndLogToConsoleAndGetErrorCode(std::function<int32_t()> lambdaFunc)
	{
		auto start = std::chrono::high_resolution_clock::now();
		auto errorCode = lambdaFunc();
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
		std::cout << "Spent time = " << duration << std::endl;
		return errorCode;
	}
}
