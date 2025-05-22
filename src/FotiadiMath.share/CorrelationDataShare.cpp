#include "CorrelationDataShare.h"

CorrelationDataShare::CorrelationDataShare(const std::vector<CoordinatesInCube>& allCoordsOfPoints,
    const std::vector<S3DEdge>& graphEdges,
    const int32_t& inlineCount,
    const int32_t& crosslineCount)
        : _graphEdges(graphEdges), 
            _crosslineCount(crosslineCount),
            _inlineCount(inlineCount)
{
    for (auto& key : allCoordsOfPoints)
    {
        _knownTraceCoordinates.insert(key);
    }
}

bool CorrelationDataShare::isContainPoint(const CoordinatesInCube& coords) const
{
    if (_knownTraceCoordinates.find(coords) != _knownTraceCoordinates.end()) {
        return true;
    }

    return false;
}

int32_t CorrelationDataShare::GetInlineCount() const
{
    return _inlineCount;
}

int32_t CorrelationDataShare::GetCrosslineCount() const
{
    return _crosslineCount;
}

void CorrelationDataShare::SaveGraphToCsv() const
{
    std::string fileNameForPointsCsv = "points.csv";
    std::ofstream pointsCsv(fileNameForPointsCsv.c_str());

    if (pointsCsv.is_open()) {
        pointsCsv << "x,y\n";
        for (auto coord : _knownTraceCoordinates)
        {
            pointsCsv << coord.inlineId << ',' << coord.crosslineId << '\n';
        }

        pointsCsv.close();
    }
    else {
        std::cerr << "Ошибка открытия файла для записи" << std::endl;
    }

    std::string fileNameForEdgesCsv = "edges.csv";
    std::ofstream edgesCsv(fileNameForEdgesCsv.c_str());

    if (edgesCsv.is_open()) {
        edgesCsv << "startX,startY,endX,endY\n";
        for (const S3DEdge& edge : _graphEdges)
        {
            edgesCsv << edge.GetPointFrom().inlineId << ',' << edge.GetPointFrom().crosslineId << ','
            << edge.GetPointTo().inlineId << ',' << edge.GetPointTo().crosslineId << '\n';
        }

        edgesCsv.close();
    }
    else {
        std::cerr << "Ошибка открытия файла для записи" << std::endl;
    }
}

int32_t CorrelationDataShare::GetDepthOfTree(int32_t inlineRef, int32_t crosslineRef) const
{
    if(_graphEdges.empty())
        return 0;

    // построение списка смежности
    std::unordered_map<int32_t, std::vector<int>> adjancyList;
    int root = -1;
    for(const auto& edge : _graphEdges)
    {
        auto pointFromId = edge.GetOneDimensionalIdOfFromPoint(_crosslineCount);
        auto pointToId = edge.GetOneDimensionalIdOfToPoint(_crosslineCount);

        auto pointFrom = edge.GetPointFrom();
        if(pointFrom.inlineId == inlineRef && pointFrom.crosslineId == crosslineRef)
        {
            // с этой вершины начнется обход графа
            // root = edge.GetIdWherePointFromStarts();
            root = pointFromId;
        }

        adjancyList[pointFromId].push_back(pointToId);
        // т.к. граф неориентированный
        adjancyList[pointToId].push_back(pointFromId);
    }

    if(root == -1)
        throw new FotiadiMath_Exceptions::ArgumentIllegalException();

    auto numOfVertexAtGraph = _inlineCount * _crosslineCount;
    std::queue<std::pair<int32_t, int32_t>> q; // {узёл индекс, глубина}
    q.push({root, 1});
    std::vector<bool> visited(numOfVertexAtGraph, false);
    visited[root] = true;
    int maxDepth = 1;

    while(!q.empty())
    {
        auto pair = q.front();
        auto currentNode = pair.first;
        auto currentDepth = pair.second;
        q.pop();
        maxDepth = std::max(maxDepth, currentDepth);

        // TODO: n.romanov бесконечный цикл
        for(int neighbour : adjancyList[currentNode])
        {
            if(neighbour != root && adjancyList[neighbour].size() > 1 && visited[neighbour] == false)
            {
                q.push({neighbour, currentDepth + 1});
                visited[neighbour] = true;
            }
        }
    }

    return maxDepth;
}

void CorrelationDataShare::SaveDepthOfTreeToFile(int32_t inlineRef, int32_t crosslineRef) const
{
    auto depth = this->GetDepthOfTree(inlineRef, crosslineRef);
    std::string fileName = "treeDepth.txt";
    std::ofstream file(fileName.c_str());

    if (file.is_open()) {
        file << depth;
        file.close();
    }
    else {
        std::cerr << "Ошибка открытия файла для записи" << std::endl;
    }
}

const std::vector<const S3DEdge*>* CorrelationDataShare::GetEdgesFromPointWithCoordinates(
    const CoordinatesInCube& pointCoords) const
{
    auto result = new std::vector<const S3DEdge*>();
    for(const S3DEdge& edge : _graphEdges)
    {
        if(edge.GetPointFrom() == pointCoords || edge.GetPointTo() == pointCoords)
        {
            result->push_back(&edge);
        }
    }

    return result;
}
