#pragma once

#include <vector>

class HorizonWrapper
{
	public:
		HorizonWrapper(
			const int32_t& numOfHorizons, // изначально число границ предполагалось = число сигналов в трассе
			const int32_t& sizeOfOneHorizon, // crosslineCount * inlineCount
			const int32_t& crosslineCount,
			const int32_t& inlineCount);

		void SetHorizon(const int32_t& x, const int32_t& y, const int32_t& horizonNum, const int32_t& value);
		
		int32_t GetHorizon(const int32_t& x, const int32_t& y, const int32_t& horizonNum) const;

		int32_t* GetHorizonsData() const;

		static const int32_t NothingValue = std::numeric_limits<int32_t>::min();

	private:
		std::vector<int32_t> _horizons;
		const int32_t _crosslineCount;
		const int32_t _inlineCount;

};
