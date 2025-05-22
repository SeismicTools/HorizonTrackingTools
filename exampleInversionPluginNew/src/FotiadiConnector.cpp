#include "FotiadiConnector.h"
#include <PathHelper.h>
#include <iostream>
#include "FileSaverHelper.h"
#include "FotiadiMathReturnCodes.h"
#include <string>

FotiadiConnector::FotiadiConnector(InversionResultInterface* resultData, 
	InversionCubeDataInterface* cube1,
	const int32_t& max_shift_point_idx,
	PluginProgressReporterInterface* callback
	) :
		_resultData(resultData),
		_cube(cube1),
		_max_shift_point_idx(max_shift_point_idx),
		_inlineCount(resultData->ilCount()),
		//_inlineCount(50),
		//_crosslineCount(2),
		_crosslineCount(50),
		//_crosslineCount(resultData->clCount()),
		_callback(callback)
{
	
}

FotiadiConnector::FotiadiConnector(InversionResultInterface* resultData, 
	InversionCubeDataInterface* cube1, 
	const int32_t& max_shift_point_idx, 
	PluginProgressReporterInterface* callback, 
	AppSettings appSettings
	) :
		_resultData(resultData),
		_cube(cube1),
		_max_shift_point_idx(max_shift_point_idx),
		_inlineCount(appSettings.InlineCount),
		_crosslineCount(appSettings.CrosslineCount),
		_callback(callback),
		_r_idx(appSettings.Ridx)
{
	auto totalIlCount = resultData->ilCount(); // 121
	if(appSettings.InlineCount > totalIlCount)
	{
		_inlineCount = totalIlCount;
	}
	else if(appSettings.InlineCount <= 0)
	{
		_inlineCount = 1;
	}

	auto totalClCount = resultData->clCount(); // 231
	if(appSettings.CrosslineCount > totalClCount)
	{
		_crosslineCount = totalClCount;
	}
	else if(appSettings.CrosslineCount <= 0)
	{
		_crosslineCount = 1;
	}

	if(appSettings.RefTraceInline >= appSettings.InlineCount || appSettings.RefTraceInline < 0)
	{
		_inlineOfRefTrace = 0;
	}
	else
	{
		_inlineOfRefTrace = appSettings.RefTraceInline;
	}

	if (appSettings.RefTraceCrossline >= appSettings.CrosslineCount || appSettings.RefTraceCrossline < 0)
	{
		_crosslineOfRefTrace = 0;
	}
	else
	{
		_crosslineOfRefTrace = appSettings.RefTraceCrossline;
	}
	_horizonsBreadth = appSettings.HorizonsBreadth;
	_spaceBetweenHorizons = appSettings.SpaceBetweenHorizons;
}

int32_t SetProgressAndGetCancellatonStatus(const double value, const void* data)
{
	auto progressReporter = (PluginProgressReporterInterface*)data;

	int32_t progress = value * 1000;
	progressReporter->setPorgressValue(progress);
	auto wasCancelled = progressReporter->wasCanceled();
	return wasCancelled == true ? 1 : 0;
}

bool FotiadiConnector::DrawS3DCubeWithReflectionHorizons()
{
	HINSTANCE dllHandler;
	void* correlationDataObject = nullptr;
	int32_t* out_surfaceIds = nullptr;
	try
	{
		dllHandler = TryToLoadFotiadyMathDll();

		TryToSetUpDllFuncs(dllHandler);

		_callback->setProgressText("Building spanning tree");
		BuildS3DCorrelationObj(&correlationDataObject);

		//auto statusCode = _defineHorizonsFunc(correlationDataObject, 0, 50, &out_surfaceIds);
		//if (statusCode != 0)
		//{
		//	std::string bla = "FM_S3DDefineReflectingHorizonsForEntireTrace error...";
		//	qDebug() << bla.c_str();
		//	throw std::runtime_error(bla);
		//}

		_callback->setProgressText("Defining horizons");
		_callback->setPorgressValue(0);
		FotiadiMathFunctions::setPorgressValueFunc funcToSetAndCheckProgress
			= &SetProgressAndGetCancellatonStatus;
		// TODO: n.romanov выставление прогресса
		auto statusCode = _defineHorizonsFuncNew(
			correlationDataObject, 
			_inlineOfRefTrace, // inline ref трассы
			_crosslineOfRefTrace, // кросслайн ref трассы
			_countOfFixedBorders,
			_surface_idx_for_reference_trace->data(),
			_spaceBetweenHorizons, // spacesBetweenTwoHorizons
			3, // sizeOfOneHorizon
			(const void*)_callback,
			funcToSetAndCheckProgress,
			_horizonsBreadth,
			&out_surfaceIds);

		if (statusCode == FM_CALCULATIONS_STOPPED_BY_UI)
		{
			std::string sth("Execution of FM_S3DDefineReflectingHorizonsForEntireTraceNew was stopped by UI.");
			qDebug() << sth.c_str();
			throw std::runtime_error(sth);
		}
		else if (statusCode != 0)
		{
			std::string bla = "FM_S3DDefineReflectingHorizonsForEntireTraceNew error...";
			qDebug() << bla.c_str();
			throw std::runtime_error(bla);
		}

		_callback->setProgressText("Saving cube");
		_callback->setPorgressValue(0);
		SaveCube(out_surfaceIds);

		_freeFunc(correlationDataObject);
		_freeArrayFunc(out_surfaceIds);
		FreeLibrary(dllHandler);
	}
	catch (const std::exception& e)
	{
		if (correlationDataObject != nullptr)
		{
			_freeFunc(correlationDataObject);
		}

		if (out_surfaceIds != nullptr)
		{
			_freeArrayFunc(out_surfaceIds);
		}

		if (!dllHandler)
		{
			FreeLibrary(dllHandler);
		}

		return false;
	}

	return true;
}

bool FotiadiConnector::DrawS3DCubeWithReflectionHorizonsOneToAllTraces()
{
	HINSTANCE dllHandler;
	void* correlationDataObject = nullptr; // TODO: n.romanov когда начну её заполнять, надо будет делать free

	try
	{
		dllHandler = TryToLoadFotiadyMathDll();
		TryToSetUpDllFuncsForOneToManyWay(dllHandler);

		CallOneToManyCorrelationAlgo(&correlationDataObject);

		FreeLibrary(dllHandler);
	}
	catch(const std::exception& e)
	{
		if (!dllHandler)
		{
			FreeLibrary(dllHandler);
		}
		return false;
	}

	return true;
}

FotiadiConnector::~FotiadiConnector()
{
	// TODO: n.romanov ВОПРОС почему вызывается два раза?
	delete _surface_idx_for_reference_trace;
	delete [] _surface_idx;
	// TODO: n.romanov вернуть
	// TODO: n.romanov все границы забивать 0 и max value, если -1?
	delete [] _fix_point_idx_r; 
}

HINSTANCE FotiadiConnector::TryToLoadFotiadyMathDll()
{
	const std::string relativePathToFotiadiMathDll
		= "\\plugins\\fotiadi_math_internal\\FotiadiMath.win.dll";

	std::string pathToFotiadiMathDll
		= PathHelper::GetFullPathRelativeStartUpDirectoryOfApplication(relativePathToFotiadiMathDll);

	HINSTANCE hDLL = LoadLibrary(pathToFotiadiMathDll.c_str());
	if (!hDLL) {
		std::string text = std::string("Error occured when load ") + pathToFotiadiMathDll;
		qCritical() << text.c_str();
		throw std::runtime_error(text);
	}

	return hDLL;
}

void FotiadiConnector::TryToSetUpDllFuncs(HINSTANCE dllRef)
{
	std::string text;

	_correlationObjBuilder = (FotiadiMathFunctions::FM_Seismic3DCorrelation)GetProcAddress(dllRef,
		"FM_Seismic3DCorrelation");
	if (!_correlationObjBuilder) {
		text = "could not locate the FM_Seismic3DCorrelation function in the dynamic library FotiadiMath.win.dll";
		qCritical() << text.c_str();
		throw std::runtime_error(text);
	}

	_freeFunc = (FotiadiMathFunctions::FM_S3D_Free)GetProcAddress(dllRef, "FM_S3D_Free");
	if (!_freeFunc) {
		text = "could not locate the FM_S3D_Free function in the dynamic library FotiadiMath.win.dll";
		qCritical() << text.c_str();
		throw std::runtime_error(text);
	}

	_freeArrayFunc = (FotiadiMathFunctions::FM_S3D_FreeArray)GetProcAddress(dllRef, "FM_S3D_FreeArray");
	if (!_freeArrayFunc) {
		text = "could not locate the FM_S3D_FreeArray function in the dynamic library FotiadiMath.win.dll";
		qCritical() << text.c_str();
		throw std::runtime_error(text);
	}

	_defineHorizonsFunc
		= (FotiadiMathFunctions::FM_S3DDefineReflectingHorizonsForEntireTrace)GetProcAddress(dllRef, "FM_S3DDefineReflectingHorizonsForEntireTrace");
	if (!_defineHorizonsFunc)
	{
		text = "could not locate the FM_S3DDefineReflectingHorizonsForEntireTrace function in the dynamic library FotiadiMath.win.dll";
		qCritical() << text.c_str();
		throw std::runtime_error(text);
	}

	_defineHorizonsFuncNew
		= (FotiadiMathFunctions::FM_S3DDefineReflectingHorizonsForEntireTraceNew)GetProcAddress(dllRef,
			"FM_S3DDefineReflectingHorizonsForEntireTraceNew");
	if (!_defineHorizonsFuncNew)
	{
		text = "could not locate the FM_S3DDefineReflectingHorizonsForEntireTraceNew function in the dynamic library FotiadiMath.win.dll";
		qCritical() << text.c_str();
		throw std::runtime_error(text);
	}
}

void FotiadiConnector::TryToSetUpDllFuncsForOneToManyWay(HINSTANCE dllRef)
{
	std::string text;

	_oneToManyCorrelationObjBuilder = (FotiadiMathFunctions::FM_Seismic3DCorrelation_one_to_many)GetProcAddress(dllRef,
		"FM_Seismic3DCorrelation_one_to_many");
	if (!_oneToManyCorrelationObjBuilder) {
		text = "could not locate the FM_Seismic3DCorrelation_one_to_many function in the dynamic library FotiadiMath.win.dll";
		qCritical() << text.c_str();
		throw std::runtime_error(text);
	}

	_freeArrayFunc = (FotiadiMathFunctions::FM_S3D_FreeArray)GetProcAddress(dllRef, "FM_S3D_FreeArray");
	if (!_freeArrayFunc) {
		text = "could not locate the FM_S3D_FreeArray function in the dynamic library FotiadiMath.win.dll";
		qCritical() << text.c_str();
		throw std::runtime_error(text);
	}
}

void FotiadiConnector::BuildS3DCorrelationObj(void** out_correlationData)
{
	// рассчитываем число сигналов в одной трассе
	// число сигналов в одной трассе = число сигналов в самой длинной трассе
	//SetNumOfSignalsAtOneTraceToMaxValue();
	SetNumOfSignalsAtOneTraceToMinValue();

	SetStartGlobalId();
	// TODO: n.romanov взять за правило - первыми всегда идут inline`ы!!!

	auto allSignals = GetAllSignals();

	int numOfTraces = _inlineCount;
	//FileSaverHelper::SaveTraceToCsv(numOfTraces, 
	//	*allSignals, 
	//	_numOfSignalsAtOneTrace, 
	//	"./plugins/fotiadi_math_internal/saved_traces/"
	//);

	SetFixedBorders();

	FotiadiMathFunctions::setPorgressValueFunc funcToSetAndCheckProgress 
		= &SetProgressAndGetCancellatonStatus;

	// TODO: n.romanov остановка алгоритма почему-то не работает
	auto status = _correlationObjBuilder(_inlineCount,
		_crosslineCount,
		_numOfSignalsAtOneTrace,
		allSignals->data(),
		_max_shift_point_idx,
		_cube->horizonsCount(),
		_surface_idx,
		_fix_point_idx_r,
		(const void*)_callback,
		funcToSetAndCheckProgress,
		_r_idx,
		out_correlationData
	);

	delete allSignals;

	if(status == FM_CALCULATIONS_STOPPED_BY_UI)
	{
		std::string sth("Execution of FM_Seismic3DCorrelation was stopped by UI.");
		qDebug() << sth.c_str();
		throw std::runtime_error(sth);
	}
	else if (status != FM_ERROR_NO)
	{
		std::string sth("Status of calling FM_Seismic3DCorrelation doesn't indicate success.");
		qDebug() << sth.c_str();
		throw std::runtime_error(sth);
	}
}

void FotiadiConnector::SetNumOfSignalsAtOneTraceToMaxValue()
{
	for (uint16 il = 0; il < _inlineCount; ++il)
	{
		for (uint16 cl = 0; cl < _crosslineCount; ++cl)
		{
			int32_t temp = _cube->endSample(il, cl) - _cube->beginSample(il, cl);
			if (temp > _numOfSignalsAtOneTrace)
			{
				_numOfSignalsAtOneTrace = temp;
			}
		}
	}
}

void FotiadiConnector::SetNumOfSignalsAtOneTraceToMinValue()
{
	_numOfSignalsAtOneTrace = 2147483647;
	for (uint16 il = 0; il < _inlineCount; ++il)
	{
		for (uint16 cl = 0; cl < _crosslineCount; ++cl)
		{
			int32_t temp = _cube->endSample(il, cl) - _cube->beginSample(il, cl);
			if (temp < _numOfSignalsAtOneTrace)
			{
				_numOfSignalsAtOneTrace = temp;
			}
		}
	}
}

void FotiadiConnector::SetStartGlobalId()
{
	_globalMinId = 2147483640;

	for (uint16 il = 0; il < _inlineCount; ++il)
	{
		for (uint16 cl = 0; cl < _crosslineCount; ++cl)
		{
			if (_resultData->isTraceExist(il, cl))
			{
				uint16 sBegin = _cube->beginSample(il, cl);
				if(sBegin < _globalMinId)
				{
					_globalMinId = sBegin;
				}
			}
		}
	}
}

void FotiadiConnector::SetFixedBorders(int32_t countOfFixedBorders, int16_t* surface_idx)
{
	_countOfFixedBorders = countOfFixedBorders;
	_surface_idx_for_reference_trace = new std::vector<int16_t>(countOfFixedBorders);
	for(int i = 0; i < countOfFixedBorders; i++)
	{
		_surface_idx_for_reference_trace->operator[](i) = surface_idx[i];
	}
}

void FotiadiConnector::SetSBeginAndEnd(uint16 sBegin, uint16 sEnd)
{
	_sBegin = sBegin;
	_sEnd = sEnd;
	_isSBeginAndSEndSet = true;
}

// вытаскиваем все сигналы из куба в одномерный массив. Недостающие сигналы заполняем NaN`ами
std::vector<double>* FotiadiConnector::GetAllSignals()
{
	auto allSignals = new std::vector<double>(_inlineCount * _crosslineCount * _numOfSignalsAtOneTrace);
	int32_t counter = 0;
	auto nanDouble = std::numeric_limits<double>::quiet_NaN();
	for (uint16 cl = 0; cl < _crosslineCount; ++cl)
	{
		for (uint16 il = 0; il < _inlineCount; ++il)
		{
			if (_resultData->isTraceExist(il, cl))
			{
				// рассчёт диапазона не бесплатный - лучше не использовать в условиях цикла
				if (_isSBeginAndSEndSet == false)
				{
					uint16 sBegin = _cube->beginSample(il, cl); // 800 (если начинать от 1700)
					uint16 sEnd = _cube->endSample(il, cl); // 951 (если брать до 1900)
					SetSBeginAndEnd(sBegin, sEnd);
				}
				for (uint16 s = _sBegin, i = 0; s < _sEnd || i < _numOfSignalsAtOneTrace; ++s, ++i)
				{
					if (_numOfSignalsAtOneTrace == i)
					{
						break; // отрезаю хвост, если трасса слишком длинная
					}
					if (s < _sEnd)
					{
						double v1 = _cube->traceData(il, cl, s);
						allSignals->operator[](counter) = v1;
						counter++;
					}
					else
					{
						allSignals->operator[](counter) = nanDouble;
						counter++;
					}
				}
			}
			else
			{
				for (int32_t i = 0; i < _numOfSignalsAtOneTrace; ++i)
				{
					allSignals->operator[](counter) = nanDouble;
					counter++;
				}
			}
		}
	}

	return allSignals;
}

void FotiadiConnector::SetFixedBorders()
{
	const int32_t countOfFixedBorders = _cube->horizonsCount();
	const int16_t radius_value = 0; // TODO: n.romanov какой радиус фиксации ставить?

	auto sizeOfArrays = _crosslineCount * _inlineCount * countOfFixedBorders;
	auto surface_ids_vector = new std::vector<int16_t>(sizeOfArrays);
	auto fix_point_idx_r_vector = new std::vector<int16_t>(sizeOfArrays);
	if (countOfFixedBorders != 0)
	{
		int32_t counter = 0;
		for (int numOfHorizon = 0; numOfHorizon < countOfFixedBorders; numOfHorizon++)
		{
			for (int cl = 0; cl < _crosslineCount; cl++)
			{
				for (int il = 0; il < _inlineCount; il++)
				{
					// почему globalmin = 800???
					// где лежит граница?
					// где начинается трасса?
					bool horizonValue_ok;
					float v = _cube->horizonValue(numOfHorizon, il, cl, horizonValue_ok);
					uint16 horizonSample = 0; // индекс границы в трассе
					if (horizonValue_ok)
					{
						horizonSample = v / _cube->sampleIntervalMs();
						horizonSample = horizonSample - _globalMinId;
						surface_ids_vector->operator[](counter) = horizonSample;
						fix_point_idx_r_vector->operator[](counter) = radius_value;
						if(counter >= sizeOfArrays)
						{
							throw std::runtime_error("");
						}
						counter++;
					}
					else
					{
						surface_ids_vector->operator[](counter) = -1;
						fix_point_idx_r_vector->operator[](counter) = radius_value;
						counter++;
					}
				}
			}
		}

		// сначала идут индексы по inline`ам, потом по crossline`ам, потом уже начинается следующая граница
		int16_t* new_array = new int16_t[sizeOfArrays];
		std::copy(surface_ids_vector->data(), surface_ids_vector->data() + sizeOfArrays, new_array);
		_surface_idx = new_array;
		delete surface_ids_vector;

		new_array = new int16_t[sizeOfArrays];
		std::copy(fix_point_idx_r_vector->data(), fix_point_idx_r_vector->data() + sizeOfArrays, new_array);
		_fix_point_idx_r = new_array;
		delete fix_point_idx_r_vector;

		std::vector<int16_t> surface_ids_for_reference_trace(countOfFixedBorders);
		for (int i = 0; i < countOfFixedBorders; i++)
		{
			bool horizonValue_ok;
			float v = _cube->horizonValue(i, _inlineOfRefTrace, _crosslineOfRefTrace, horizonValue_ok);
			uint16 horizonSample = 0;
			if (horizonValue_ok)
			{
				horizonSample = v / _cube->sampleIntervalMs();
				horizonSample = horizonSample - _globalMinId;
				surface_ids_for_reference_trace[i] = horizonSample;
			}
			else
			{
				surface_ids_for_reference_trace[counter] = -1;
			}
		}
		SetFixedBorders(countOfFixedBorders, surface_ids_for_reference_trace.data());
	}
}

void FotiadiConnector::CallOneToManyCorrelationAlgo(void** out_correlationData)
{
	// рассчитываем число сигналов в одной трассе
// число сигналов в одной трассе = число сигналов в самой длинной трассе
	SetNumOfSignalsAtOneTraceToMinValue();

	SetStartGlobalId();
	// TODO: n.romanov взять за правило - первыми всегда идут inline`ы!!!

	auto allSignals = GetAllSignals();

	int numOfTraces = _inlineCount;
	//FileSaverHelper::SaveTraceToCsv(numOfTraces,
	//	*allSignals,
	//	_numOfSignalsAtOneTrace,
	//	"./plugins/fotiadi_math_internal/saved_traces/"
	//);

	SetFixedBorders();

	// TODO: n.romanov как идея, считывать различные параметры из конфиг-файла

	FotiadiMathFunctions::setPorgressValueFunc funcToSetAndCheckProgress
		= &SetProgressAndGetCancellatonStatus;

	auto status = _oneToManyCorrelationObjBuilder(
		_inlineCount,
		_crosslineCount,
		_numOfSignalsAtOneTrace,
		allSignals->data(),
		_max_shift_point_idx,
		_cube->horizonsCount(),
		_surface_idx,
		_fix_point_idx_r,
		(const void*)_callback,
		funcToSetAndCheckProgress,
		_r_idx,
		_inlineOfRefTrace,
		_crosslineOfRefTrace,
		_horizonsBreadth,
		_spaceBetweenHorizons,
		out_correlationData
	);

	if (status == FM_CALCULATIONS_STOPPED_BY_UI)
	{
		std::string sth("Execution of FM_Seismic3DCorrelation was stopped by UI.");
		qDebug() << sth.c_str();
		throw std::runtime_error(sth);
	}

	const int32_t* horizonsData = static_cast<const int32_t*>(*out_correlationData);

	_callback->setProgressText("Saving cube");
	_callback->setPorgressValue(0);
	SaveCube(horizonsData);

	_freeArrayFunc(*out_correlationData);

	delete allSignals;

	if (status != FM_ERROR_NO)
	{
		std::string sth("Status of calling FM_Seismic3DCorrelation doesn't indicate success.");
		qDebug() << sth.c_str();
		throw std::runtime_error(sth);
	}
}

void FotiadiConnector::SaveCube(const int32_t* out_surfaceIds)
{
	// start так можно получить все границы на определенной глубине, определенного crossline`а
	//std::vector<int32_t> forDubug(_inlineCount * _crosslineCount);
	//uint16 clTest = 0;
	//uint16 depth = _numOfSignalsAtOneTrace/2;
	//for(uint16 il = 0; il < _inlineCount; ++il)
	//{
	//	auto value = out_surfaceIds[_inlineCount * _crosslineCount * depth + _inlineCount * clTest + il];
	//	forDubug[il] = value;
	//}
	// end

	int32_t NothingValue = -2147483648; // int32_t min
	int32_t finishedTracesCount = 0;
	int32_t totalCount = _inlineCount * _crosslineCount;
	int32_t totalPointCount = totalCount * _numOfSignalsAtOneTrace;
	for (uint16 il = 0; il < _inlineCount; ++il)
	{
		for (uint16 cl = 0; cl < _crosslineCount; ++cl)
		{
			if (_resultData->isTraceExist(il, cl))
			{
				for (int32_t i = 0; i < _numOfSignalsAtOneTrace; i++) // обход в глубину
				{
					auto idAtArray = _inlineCount * _crosslineCount * i
						+ _inlineCount * cl
						+ il;
					if(idAtArray < 0 || idAtArray >= totalPointCount)
					{
						int a = 5+5;
						continue;
					}
					auto value = out_surfaceIds[idAtArray];

					if(value == NothingValue)
					{
						auto bla = nanf("");
						// почему-то если ставить nan`ы, то всё становится nan`ами
						//_resultData->setResultSample(il, cl, _sBegin + i, bla);
					}
					else
					{
						_resultData->setResultSample(il, cl, _sBegin + i, static_cast<float>(value));
					}
				}

				finishedTracesCount++;
				//(double)finishedTracesCount / (double)totalCount;
			}
		}
	}
}
