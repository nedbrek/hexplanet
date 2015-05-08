#include "fill.h"
#include "neighbors.h"
#include "../src/hexplanet.h"
#include "../src/map_data.h"
#include <cstdlib>

Fill::Fill(const Neighbors &neighbors, const HexPlanet &planet)
	: neighbors_(neighbors)
	, planet_(planet)
	, fillData_(new MapData<uint8_t>)
{
}

void Fill::init(uint32_t numHexes)
{
	fillData_->init(-1, numHexes);
}

void Fill::write(FILE *f) const
{
	fillData_->write(f);
}

void Fill::plantSeed(uint32_t tileNum, uint8_t value)
{
	if ((*fillData_)[tileNum] != uint8_t(-1))
		return;

	(*fillData_)[tileNum] = value;
	plantNeighbors(tileNum);
}

void Fill::plantNeighbors(uint32_t tileNum)
{
	std::vector<uint32_t> nbrs;
	neighbors_.getNeighbors(tileNum, nbrs);

	const uint8_t value = (*fillData_)[tileNum];

	for (size_t i = 0; i < nbrs.size(); ++i)
	{
		const uint32_t triIndex = nbrs[i];
		if (triIndex == uint32_t(-1))
			continue;

		const HexTri &tri = planet_.m_hexdual[triIndex];
		plantTriVert(tri.m_hexA, value);
		plantTriVert(tri.m_hexB, value);
		plantTriVert(tri.m_hexC, value);
	}
}

void Fill::plantTriVert(uint32_t nt, uint8_t value)
{
	if ((*fillData_)[nt] == uint8_t(-1))
	{
		(*fillData_)[nt] = value;
		nextTiles_.push(FillData(rand(), nt));
	}
}

void Fill::fill()
{
	while (!nextTiles_.empty())
	{
		FillData fd = nextTiles_.top();
		nextTiles_.pop();

		plantNeighbors(fd.tileNum);
	}
}

