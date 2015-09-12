#include "../utils/fill.h"
#include "../utils/neighbors.h"
#include "../src/hexplanet.h"
#include "../src/map_data.h"
#include <fstream>
#include <cstdlib>

const char *const SPHERE_TO_USE = "sphere6.fixed.obj";
const char *const PLATE_FILE = "plate6.bin";
const char *const TERRAIN_FILE = "terrain6.bin";

void writePlates()
{
	std::ifstream is(SPHERE_TO_USE);
	HexPlanet p;
	p.read(is);
	p.projectToSphere();
	const uint32_t numTiles = p.getNumHexes();

	Neighbors neighbors;
	neighbors.init(p);

	Fill f(neighbors, p);
	f.init(numTiles);

	const uint32_t numSeeds = 50;
	for (uint32_t i = 0; i < numSeeds; ++i)
	{
		const uint32_t tile = rand() % numTiles;
		f.plantSeed(tile, i);
	}

	f.fill();

	FILE *of = fopen(PLATE_FILE, "wb");
	f.write(of);
}

void writeTerrain()
{
	std::ifstream is(SPHERE_TO_USE);
	HexPlanet p;
	p.read(is);
	p.projectToSphere();
	const uint32_t numTiles = p.getNumHexes();

	FILE *f = fopen(TERRAIN_FILE, "wb");
	fwrite(&numTiles, 4, 1, f);
	for (uint32_t i = 0; i < numTiles; ++i)
	{
		const uint8_t d = rand() % 5;
		fwrite(&d, 1, 1, f);
	}
	fclose(f);

	f = fopen(TERRAIN_FILE, "rb");
	MapData<uint8_t> map;
	const int ret = map.read(f);
	if (ret != 0)
		printf("Read failed. Code %d\n", ret);
	else
		printf("Read successful.\n");
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Usage %s <t|p>\n", argv[0]);
		return 1;
	}

	if (argv[1][0] == 't')
		writeTerrain();
	else if (argv[1][0] == 'p')
		writePlates();
	else
	{
		printf("Usage %s <t|p>\n", argv[0]);
		return 1;
	}

	return 0;
}

