#include "../src/hexplanet.h"
#include <cstdlib>

int main(int argc, char **argv)
{
	int level = 0;
	if (argc > 1)
	{
		level = atoi(argv[1]);
	}
	HexPlanet planet(level, 0, 0);
	planet.write(std::cout);
	return 0;
}

