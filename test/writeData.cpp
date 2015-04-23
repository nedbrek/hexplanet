#include "../src/map_data.h"
#include <cstdlib>

int main(int argc, char **argv)
{
	const uint32_t numTiles = 7292; // sphere 6

	FILE *f = fopen("terrain6.bin", "wb");
	fwrite(&numTiles, 4, 1, f);
	for (uint32_t i = 0; i < numTiles; ++i)
	{
		const uint8_t d = rand() % 5;
		fwrite(&d, 1, 1, f);
	}
	fclose(f);

	f = fopen("terrain6.bin", "rb");
	MapData<uint8_t> map;
	const int ret = map.read(f);
	if (ret != 0)
		printf("Read failed. Code %d\n", ret);
	else
		printf("Read successful.\n");

	return 0;
}

