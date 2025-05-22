#pragma once
#include <string>
#include <vector>

class Seismic3DCube
{
	public:
		Seismic3DCube(const int32_t& ilCount,
			const int32_t& clCount,
			const int32_t& numOfSignalsAtOneTrace,
			const std::vector<double>& signalsVar );

		static Seismic3DCube* CreateUninitializedInstance();

		~Seismic3DCube();

		const double* TryToGetAllCubeData() const;

		const int32_t TryToGetInlineCount() const;

		const int32_t TryToGetCrosslineCount() const;

		const int32_t TryToGetNumOfSignalsAtOneTrace() const;

		friend std::ostream& operator<<(std::ostream& os, const Seismic3DCube& obj);

		friend std::istream& operator>>(std::istream& in, Seismic3DCube& obj);

	private:
		const std::vector<double>* _signals = nullptr;
		int32_t _ilCount;
		int32_t _clCount;
		int32_t _numOfSignalsAtOneTrace;
		bool _isInitialized;

		static const std::string s_c_cubeWasntInitializedTextError;

		Seismic3DCube();

		void ThrowErrorIfCubeWasntInitialized() const;
};
