#pragma once

#include <cstdint>

struct AdjacencyListUnit
{
	double weight;
	int32_t vertexId;

	AdjacencyListUnit(double w, int32_t v) : weight(w), vertexId(v)
	{

	}

	bool operator<(const AdjacencyListUnit& other) const 
	{
		return weight < other.weight;
	}

	bool operator>(const AdjacencyListUnit& other) const
	{
		return weight > other.weight;
	}
};
