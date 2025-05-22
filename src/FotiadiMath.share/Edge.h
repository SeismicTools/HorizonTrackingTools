#pragma once

#include <cstdint>

struct Edge
{
	int32_t _vertexIdFrom;
	int32_t _vertexIdTo;
	double _weight;

	Edge(int32_t vertexIdFrom, int32_t vertexIdTo, double weight)
		: _vertexIdFrom(vertexIdFrom), _vertexIdTo(vertexIdTo), _weight(weight)
	{

	}

	bool operator<(const Edge& other) const
	{
		return _weight < other._weight;
	}

	bool operator>(const Edge& other) const
	{
		return _weight > other._weight;
	}

	bool operator==(const Edge& b) {
		return _weight == b._weight &&
			_vertexIdFrom == b._vertexIdFrom &&
			_vertexIdTo == b._vertexIdTo;
	}
};
