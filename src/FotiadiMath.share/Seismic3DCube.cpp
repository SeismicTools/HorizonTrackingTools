#include "Seismic3DCube.h"
#include <iostream>
#include <fstream>

const std::string Seismic3DCube::s_c_cubeWasntInitializedTextError = std::string("Cube wasn't initialized.");

Seismic3DCube::Seismic3DCube(const int32_t& ilCount,
	const int32_t& clCount,
	const int32_t& numOfSignalsAtOneTrace,
	const std::vector<double>& signals) :
		_ilCount(ilCount), _clCount(clCount),
		_numOfSignalsAtOneTrace(numOfSignalsAtOneTrace),
		_signals(&signals),
		_isInitialized(true)
{
	
}

Seismic3DCube* Seismic3DCube::CreateUninitializedInstance()
{
	return new Seismic3DCube();
}

Seismic3DCube::~Seismic3DCube()
{
	delete _signals;
}

const double* Seismic3DCube::TryToGetAllCubeData() const
{
	ThrowErrorIfCubeWasntInitialized();
	return _signals->data();
}

const int32_t Seismic3DCube::TryToGetInlineCount() const
{
	ThrowErrorIfCubeWasntInitialized();
	return _ilCount;
}

const int32_t Seismic3DCube::TryToGetCrosslineCount() const
{
	ThrowErrorIfCubeWasntInitialized();
	return _clCount;
}

const int32_t Seismic3DCube::TryToGetNumOfSignalsAtOneTrace() const
{
	ThrowErrorIfCubeWasntInitialized();
	return _numOfSignalsAtOneTrace;
}

std::ostream& operator<<(std::ostream& os, const Seismic3DCube& obj)
{
	obj.ThrowErrorIfCubeWasntInitialized();

	std::vector<int32_t> header = { obj._ilCount, obj._clCount, obj._numOfSignalsAtOneTrace };

	os.write(reinterpret_cast<const char*>(header.data()), header.size() * sizeof(int32_t));

	os.write(reinterpret_cast<const char*>(obj._signals->data())
		, obj._signals->size() * sizeof(double));

	return os;
}

std::istream& operator>>(std::istream& in, Seismic3DCube& obj)
{
	obj.~Seismic3DCube();

	in.read(reinterpret_cast<char*>(&obj._ilCount), sizeof(obj._ilCount));
	in.read(reinterpret_cast<char*>(&obj._clCount), sizeof(obj._clCount));
	in.read(reinterpret_cast<char*>(&obj._numOfSignalsAtOneTrace), sizeof(obj._numOfSignalsAtOneTrace));

	int totalNumOfAllSignalsAtCube = obj._ilCount * obj._clCount * obj._numOfSignalsAtOneTrace;
	std::vector<double>* signals = new std::vector<double>(totalNumOfAllSignalsAtCube);

	in.read(reinterpret_cast<char*>(signals->data()), totalNumOfAllSignalsAtCube * sizeof(double));
	obj._signals = signals;
	obj._isInitialized = true;

	return in;
}

Seismic3DCube::Seismic3DCube()
	: _ilCount(-1), _clCount(-1),
	_numOfSignalsAtOneTrace(-1),
	_isInitialized(false)
{

}

void Seismic3DCube::ThrowErrorIfCubeWasntInitialized() const
{
	if (!_isInitialized)
	{
		throw std::runtime_error(s_c_cubeWasntInitializedTextError);
	}
}
