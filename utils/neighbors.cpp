#include "neighbors.h"
#include "../src/hexplanet.h"

Neighbors::Neighbors()
	: neighbors_(NULL)
{
}

void Neighbors::init(const HexPlanet &planet)
{
	// set up neighbor arrays (initialized to -1)
	const size_t numHexes = planet.getNumHexes();
	neighbors_ = new uint32_t*[numHexes];

	for (size_t i = 0; i < numHexes; ++i)
	{
		// at most 6 neighbors (some have 5)
		neighbors_[i] = new uint32_t[6];
		for (uint32_t j = 0; j < 6; ++j)
		{
			neighbors_[i][j] = -1;
		}
	}

	// it would be nice to use planet.getNeighbors() here, but it costs O(N)
	// (which would give us O(N^2))
	for (size_t ti = 0; ti != planet.numTriangles(); ++ti)
	{
		update(ti, planet.triangle(ti).m_hexA);
		update(ti, planet.triangle(ti).m_hexB);
		update(ti, planet.triangle(ti).m_hexC);
	}
}

void Neighbors::update(uint32_t triangleIndex, uint32_t hexIdx)
{
	uint32_t j = 0;
	for (; j < 6; ++j)
	{
		if (neighbors_[hexIdx][j] == uint32_t(-1))
			break;
		if (neighbors_[hexIdx][j] == triangleIndex)
			return;
	}

	if (j == 6)
	{
		std::cerr << "Error: neighnor information maxed out!" << std::endl;
		return;
	}
	neighbors_[hexIdx][j] = triangleIndex;
}

void Neighbors::getNeighbors(uint32_t tileIdx, std::vector<uint32_t> &nbrs) const
{
	for (uint32_t j = 0; j < 6; ++j)
	{
		const uint32_t nbr = neighbors_[tileIdx][j];
		if (nbr != uint32_t(-1))
			nbrs.push_back(nbr);
	}
}

