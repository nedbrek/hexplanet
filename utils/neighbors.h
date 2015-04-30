#ifndef HEX_PLANET_NEIGHBORS_H
#define HEX_PLANET_NEIGHBORS_H

#include <stdint.h>
#include <vector>
class HexPlanet;

class Neighbors
{
public:
	Neighbors();

	void init(const HexPlanet &planet);

	void getNeighbors(uint32_t tileIdx, std::vector<uint32_t> &nbrs);

protected: // methods
	void update(uint32_t triangleIndex, uint32_t hexIdx);

protected: // data
	uint32_t **neighbors_;
};

#endif

