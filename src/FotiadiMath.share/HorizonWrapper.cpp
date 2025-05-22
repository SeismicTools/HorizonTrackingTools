#include "HorizonWrapper.h"

HorizonWrapper::HorizonWrapper(const int32_t& numOfHorizons, 
	const int32_t& sizeOfOneHorizon,
	const int32_t& crosslineCount,
	const int32_t& inlineCount)
	: 
	_horizons(numOfHorizons * sizeOfOneHorizon, std::numeric_limits<int32_t>::min()),
	_crosslineCount(crosslineCount),
	_inlineCount(inlineCount)
{
	
}

void HorizonWrapper::SetHorizon(const int32_t& inlineId, 
	const int32_t& crosslineId, 
	const int32_t& horizonNum, // глубина, куда поставим номер границы
	const int32_t& value)
{
	auto id = inlineId
		+ crosslineId * _inlineCount
		+ horizonNum * _crosslineCount * _inlineCount;
	if(id >= _horizons.size())
	{
		return;
	}
	_horizons.operator[](id)
		= value;
}

int32_t HorizonWrapper::GetHorizon(const int32_t& inlineId, 
	const int32_t& crosslineId, 
	const int32_t& horizonNum) const
{
	return _horizons.operator[](inlineId 
		+ crosslineId * _inlineCount
		+ horizonNum * _crosslineCount * _inlineCount
	);
}

int32_t* HorizonWrapper::GetHorizonsData() const
{
	int32_t* arr = new int32_t[_horizons.size()];

	for (size_t i = 0; i < _horizons.size(); ++i) {
		arr[i] = _horizons[i];
	}

	return arr;
}
