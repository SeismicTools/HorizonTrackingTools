#pragma once

#include <iostream>
#include <vector>
#include "CoordinatesInCube.h"
#include <Seismic3DPointNeighbour.h>
#include <Point3DSimilarityHelper.h>
#include <DataToFillGraphPoints.h>
#include <unordered_map>

class Seismic3DPointWithNeighboursAndSimilarityValues {
    public:
        Seismic3DPointWithNeighboursAndSimilarityValues(const CoordinatesInCube& coordinatesInCube, 
             std::vector<double>* signals)
                : _coordinates(coordinatesInCube), 
                    _traceSignals(signals),
                    _idWhereTraceStarts(0),
                    _canCorrelateThisPoint(true)
        {
    
        }

        Seismic3DPointWithNeighboursAndSimilarityValues(const CoordinatesInCube& coordinatesInCube,
            std::vector<double>* signals,
            int32_t idWhereTraceStarts,
            bool canCorrelateThisPoint)
            : _coordinates(coordinatesInCube),
            _traceSignals(signals),
            _idWhereTraceStarts(idWhereTraceStarts),
            _canCorrelateThisPoint(canCorrelateThisPoint)
        {
            _startIdWasSet = true;
        }
    
        CoordinatesInCube getCoordinates() const;

        const CoordinatesInCube& GetCoordinatesByRef() const;
    
        size_t GetNumOfSignals() const;
    
        const double* GetSignals() const;

        const std::vector<Seismic3DPointNeighbour*>& GetNeighborsAndSimilarities() const;

        const int32_t GetIdAtOneDimensionalArray(const int32_t& numOfElemAtOneRaw) const;
    
        void addNeighboursAndCountSimilarityForThem(
            std::vector<Seismic3DPointWithNeighboursAndSimilarityValues*>& pointsToAdd,
            const DataToFillGraphPoints& data);

        void resizeTrace(const DataToFillGraphPoints& data);
    
        void addNeighbor(Seismic3DPointNeighbour* neighbor);

        const Seismic3DPointNeighbour* TryToFindNeighborPointByCoords(CoordinatesInCube neighborCoords) const;

        const Seismic3DPointNeighbour* TryToFindNeighborPointByCoordsFast(const CoordinatesInCube& neighborCoords) const;

        const int32_t GetIdWhereTraceStarts() const;

        const bool CanCorrelateThisPoint() const;

        void MakeDictionaryForNeighbours();

        ~Seismic3DPointWithNeighboursAndSimilarityValues();
    
    private:
        CoordinatesInCube _coordinates;
        std::vector<Seismic3DPointNeighbour*> _neighborsAndSimiliarity;
        std::vector<double>* _traceSignals;
        // id, с какого момента записаны сигналы из трассы
        // если начинается с нуля, значит трасса начинается с самой верхней возможной точки (от которой поданы данные)
        // чем больше id, тем ниже точка в сигнале
        int32_t _idWhereTraceStarts; 
        bool _canCorrelateThisPoint; // если нет верхней или нижней границы для трассы, то будет false
        bool _startIdWasSet = false; // _idWhereTraceStarts уже расчитан и размер трассы изменен
        std::unordered_map<CoordinatesInCube, Seismic3DPointNeighbour*>* _neighboursDict = nullptr;
};
