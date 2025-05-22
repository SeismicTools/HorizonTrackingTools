#pragma once

#include <cstdint>
#include <CorrelationDataShare.h>
#include <TypedefStuff.h>

class ReflectingHorizonDefinerForEntireTrace
{
	public:
		static int32_t* FindReflectingHorizonsForEntireTrace(
			const int32_t& traceCoordByX,
			const int32_t& traceCoordByY,
			const CorrelationDataShare& correlationData
		);

		static int32_t* FindReflectingHorizonsForEntireTrace(
			const int32_t& traceCoordByX,
			const int32_t& traceCoordByY,
			const CorrelationDataShare& correlationData,
			const int32_t countOfFixedBorders, // число отражающих горизонтов
			int16_t surface_idx[], // размер = countOfFixedBorders. Здесь лежат id, которые зафиксированы
			const int32_t spacesBetweenTwoHorizons, // растояние в индексах между двумя границами
			const int32_t sizeOfOneHorizon, // размер одной границы в индексах
			const void* callbackData, // тут будет указатель на объект класса PluginProgressReporterInterface
			setPorgressValueFunc callback,
			const int32_t horizonsBreadth			// ширина границ
		);
};
