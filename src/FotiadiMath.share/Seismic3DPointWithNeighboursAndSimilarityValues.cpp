#include "Seismic3DPointWithNeighboursAndSimilarityValues.h"

CoordinatesInCube Seismic3DPointWithNeighboursAndSimilarityValues::getCoordinates() const {
    return _coordinates;
}

const CoordinatesInCube& Seismic3DPointWithNeighboursAndSimilarityValues::GetCoordinatesByRef() const
{
    return _coordinates;
}

size_t Seismic3DPointWithNeighboursAndSimilarityValues::GetNumOfSignals() const
{
    return _traceSignals->size();
}

const double* Seismic3DPointWithNeighboursAndSimilarityValues::GetSignals() const
{
    return _traceSignals->data();
}

const std::vector<Seismic3DPointNeighbour*>& 
    Seismic3DPointWithNeighboursAndSimilarityValues::GetNeighborsAndSimilarities() const
{
    return _neighborsAndSimiliarity;
}

const int32_t 
    Seismic3DPointWithNeighboursAndSimilarityValues::GetIdAtOneDimensionalArray(const int32_t& numOfElemAtOneRaw) const
{
    return _coordinates.inlineId * numOfElemAtOneRaw + _coordinates.crosslineId;
}

void Seismic3DPointWithNeighboursAndSimilarityValues::resizeTrace(const DataToFillGraphPoints& data)
{
    if(_startIdWasSet)
        return;

    auto idsOfFixedPointsAtThisTrace = data.GetIdsOfFixedPointsForTraceWithCoordinatesWithOffset(this->getCoordinates().inlineId, this->getCoordinates().crosslineId, 0);
    auto minId = *std::min_element(idsOfFixedPointsAtThisTrace->begin(), idsOfFixedPointsAtThisTrace->end());
    auto maxId = *std::max_element(idsOfFixedPointsAtThisTrace->begin(), idsOfFixedPointsAtThisTrace->end());

    if (minId < 0 || maxId < 0)
    {
        minId = 0;
        maxId = data.GetNTime() - 1;
    }

    if (maxId >= data.GetNTime())
        maxId = data.GetNTime() - 1;

    int totalLenght;
    if (data.GetNTime() < maxId)
    {
        totalLenght = data.GetNTime() - minId + 1;
    }
    else // >=
    {
        totalLenght = maxId - minId + 1;
    }

    auto signalsForThisTrace = new std::vector<double>(totalLenght);
    int32_t idToWrite = 0;
    auto signals = data.GetSignals();
    for (int32_t t = minId; t <= maxId && t < data.GetNTime(); ++t, idToWrite++) {
        auto id = this->getCoordinates().crosslineId * data.GetNTraceY() * data.GetNTime()
            + this->getCoordinates().inlineId * data.GetNTime()
            + t;
        auto signal_value = signals[id];
        (*signalsForThisTrace)[idToWrite] = signal_value;
    }

    delete _traceSignals;
    _traceSignals = signalsForThisTrace;
    _idWhereTraceStarts = minId;
    _startIdWasSet = true;
}

void Seismic3DPointWithNeighboursAndSimilarityValues::addNeighboursAndCountSimilarityForThem
    (std::vector<Seismic3DPointWithNeighboursAndSimilarityValues*>& pointsToAdd,
        const DataToFillGraphPoints& data)
{
    for (auto& neighbourPoint : pointsToAdd)
    {
        auto idsOfFixedPointsAtXTrace = data.GetIdsOfFixedPointsForTraceWithCoordinatesWithOffset(this->getCoordinates().inlineId, this->getCoordinates().crosslineId, this->GetIdWhereTraceStarts());
        auto idsOfFixedPointsAtYTrace = data.GetIdsOfFixedPointsForTraceWithCoordinatesWithOffset(neighbourPoint->getCoordinates().inlineId, neighbourPoint->getCoordinates().crosslineId, neighbourPoint->GetIdWhereTraceStarts());
        Point3DSimilarityHelper similarityValueHelper(*this, 
            *neighbourPoint, 
            data.GetMaxShiftAtEachPointByIndexBetweenNeighboringTraces(),
            data.GetCountOfFixedBorders(),
            idsOfFixedPointsAtXTrace->data(),
            idsOfFixedPointsAtYTrace->data(),
            data.GetBordersRadiuses(),
            data.GetRIdx()
        );

        //auto similarityValue = similarityValueHelper.getSimilarityOfTwoPoints();
        auto similarityValue = similarityValueHelper.getSimilarityOfTwoPointsNotEqualsLenghtOfTraces();
        delete idsOfFixedPointsAtXTrace;
        delete idsOfFixedPointsAtYTrace;

        auto f2s = similarityValueHelper.GetMappingFirstTraceOnSecond();
        // отображение с 0 id до _numOfSignalsAtOneTrace-1
        auto normalMappingf2s = new std::vector<int16_t>(data.GetNTime(), -32768);
        for (int j = this->GetIdWhereTraceStarts(), k = 0; k < f2s.size(); k++, j++)
        {
            normalMappingf2s->operator[](j) = f2s[k];
        }

        auto neighbourWithSimilarity = new Seismic3DPointNeighbour(*neighbourPoint, 
            similarityValue,
            *normalMappingf2s);
        this->addNeighbor(neighbourWithSimilarity);

        delete normalMappingf2s;

        auto s2f = similarityValueHelper.GetMappingSecondTraceOnSFirst();
        // отображение с 0 id до _numOfSignalsAtOneTrace-1
        auto normalMappings2f = new std::vector<int16_t>(data.GetNTime(), -32768);
        for (int j = neighbourPoint->GetIdWhereTraceStarts(), k = 0; k < s2f.size(); k++, j++)
        {
            normalMappings2f->operator[](j) = s2f[k];
        }

        auto neighbourWithSimilarityForNeighbour = new Seismic3DPointNeighbour(*this,
            similarityValue,
            *normalMappings2f);
        neighbourPoint->addNeighbor(neighbourWithSimilarityForNeighbour);
        delete normalMappings2f;
    }
}

void Seismic3DPointWithNeighboursAndSimilarityValues::addNeighbor(Seismic3DPointNeighbour* neighbor)
{
    _neighborsAndSimiliarity.push_back(neighbor);
}

Seismic3DPointWithNeighboursAndSimilarityValues::~Seismic3DPointWithNeighboursAndSimilarityValues()
{
    delete _traceSignals;
    for (auto neighbour : _neighborsAndSimiliarity)
    {
        delete neighbour;
    }
    _neighborsAndSimiliarity.clear();
    if(_neighboursDict != nullptr)
    {
        delete _neighboursDict;
        _neighboursDict = nullptr;
    }
}

const Seismic3DPointNeighbour* Seismic3DPointWithNeighboursAndSimilarityValues::TryToFindNeighborPointByCoords(CoordinatesInCube neighborCoords) const
{
    for(auto neighbor : _neighborsAndSimiliarity)
    {
        auto coords = neighbor->GetCoordinates();
        if(neighborCoords == coords)
            return neighbor;
    }

    return nullptr;
}

const Seismic3DPointNeighbour* Seismic3DPointWithNeighboursAndSimilarityValues::TryToFindNeighborPointByCoordsFast(
    const CoordinatesInCube& neighborCoords) const
{
    if(_neighboursDict == nullptr)
        return nullptr;
    return (*_neighboursDict)[neighborCoords];
}

const int32_t Seismic3DPointWithNeighboursAndSimilarityValues::GetIdWhereTraceStarts() const
{
    return _idWhereTraceStarts;
}

const bool Seismic3DPointWithNeighboursAndSimilarityValues::CanCorrelateThisPoint() const
{
    return _canCorrelateThisPoint;
}

void Seismic3DPointWithNeighboursAndSimilarityValues::MakeDictionaryForNeighbours()
{
    _neighboursDict = new std::unordered_map<CoordinatesInCube, Seismic3DPointNeighbour*>();
    for (auto neighbor : _neighborsAndSimiliarity)
    {
        (*_neighboursDict)[neighbor->GetCoordinates()] = neighbor;
    }
}
