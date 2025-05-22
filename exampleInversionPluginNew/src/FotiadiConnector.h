#pragma once

#include <inversionplugininterfaces.h>
#include <windows.h>
#include <FotiadiMathFunctions.h>
#include <AppSettings.h>

class FotiadiConnector
{
	public:
		FotiadiConnector(InversionResultInterface* resultData,
			InversionCubeDataInterface* cube1,
			const int32_t& max_shift_point_idx,
			PluginProgressReporterInterface* callback);

		FotiadiConnector(InversionResultInterface* resultData,
			InversionCubeDataInterface* cube1,
			const int32_t& max_shift_point_idx,
			PluginProgressReporterInterface* callback,
			AppSettings appSettings);
		
		// способ, когда корреляция производится только между соседними трассами
		bool DrawS3DCubeWithReflectionHorizons();
		// способ, когда одна, опорная трасса, коррелируется со всеми остальными
		bool DrawS3DCubeWithReflectionHorizonsOneToAllTraces();

		~FotiadiConnector();

	private:
		InversionResultInterface* _resultData;
		InversionCubeDataInterface* _cube;
		FotiadiMathFunctions::FM_Seismic3DCorrelation _correlationObjBuilder;
		FotiadiMathFunctions::FM_Seismic3DCorrelation_one_to_many _oneToManyCorrelationObjBuilder;
		FotiadiMathFunctions::FM_S3D_Free _freeFunc;
		FotiadiMathFunctions::FM_S3D_FreeArray _freeArrayFunc;
		FotiadiMathFunctions::FM_S3DDefineReflectingHorizonsForEntireTrace _defineHorizonsFunc;
		FotiadiMathFunctions::FM_S3DDefineReflectingHorizonsForEntireTraceNew _defineHorizonsFuncNew;
		int32_t _numOfSignalsAtOneTrace = 0;
		int32_t _max_shift_point_idx;
		int32_t _inlineCount;
		int32_t _crosslineCount;
		int32_t _globalMinId = 0;
		int32_t _countOfFixedBorders = 0;
		int32_t _inlineOfRefTrace = 0;
		int32_t _crosslineOfRefTrace = 0;
		int32_t _horizonsBreadth = 0;
		int32_t _spaceBetweenHorizons = 0;
		std::vector<int16_t>* _surface_idx_for_reference_trace = NULL; // фиксированные значения опорной трассы (от которой ищем отражающие горизонты)
		PluginProgressReporterInterface* _callback;
		bool _isSBeginAndSEndSet = false;
		uint16 _sBegin = -1; // id верхней части трассы = 800 (если начинать от 1700)
		uint16 _sEnd = -1; // id нижней части трассы = 951 (если брать до 1900)
		int16_t* _surface_idx = nullptr; // зафиксированная id для каждой трассы (поверхность)
		int16_t* _fix_point_idx_r = nullptr; // радиусы фиксации для каждой точки границ
		int32_t _r_idx = 10;

		static HINSTANCE TryToLoadFotiadyMathDll();

		void TryToSetUpDllFuncs(HINSTANCE dllRef);
		// загрузка dll`ек для способа корреляции одной трассы ко многим
		void TryToSetUpDllFuncsForOneToManyWay(HINSTANCE dllRef);
		void BuildS3DCorrelationObj(void** out_correlationData);
		void SaveCube(const int32_t* out_surfaceIds);
		void SetNumOfSignalsAtOneTraceToMaxValue();
		void SetNumOfSignalsAtOneTraceToMinValue();
		void SetStartGlobalId(); // рассчитывает и выставляет значение глобальной id (в глубину трассы) в кубе
		void SetFixedBorders(int32_t countOfFixedBorders, int16_t* surface_idx);
		void SetSBeginAndEnd(uint16 sBegin, uint16 sEnd);
		std::vector<double>* GetAllSignals();
		void SetFixedBorders();
		void CallOneToManyCorrelationAlgo(void** out_correlationData);
};
