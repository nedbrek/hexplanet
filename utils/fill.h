#ifndef HEX_PLANET_FILL_H
#define HEX_PLANET_FILL_H

#include <queue>
#include <cstdio>
#include <stdint.h>

class HexPlanet;
class Neighbors;
template<typename T> class MapData;

class Fill
{
public:
	Fill(const Neighbors &neighbors, const HexPlanet &planet);

	void init(uint32_t numHexes);

	void write(FILE *f) const;

	void plantSeed(uint32_t tileNum, uint8_t value);

	void fill();

protected: // methods
	void plantNeighbors(uint32_t tileNum);
	void plantTriVert(uint32_t tileNum, uint8_t value);

protected: // data
	const Neighbors &neighbors_;
	const HexPlanet &planet_;
	MapData<uint8_t> *fillData_;

	struct FillData
	{
		int32_t weight;
		uint32_t tileNum;

		FillData(int32_t w, uint32_t t)
			: weight(w)
			, tileNum(t)
		{
		}

		bool operator<(const FillData &rhs) const
		{
			return weight < rhs.weight;
		}
	};
	std::priority_queue<FillData> nextTiles_;
};

#endif

