#include "../src/hexplanet.h"
#include <cstdlib>
#include <fstream>

int main(int argc, char **argv)
{
	int level = 0;
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << "[-l <level>]|[<filename>]" << std::endl;
		return 1;
	}

	bool hasFilename = true;
	if (argv[1][0] == '-')
	{
		level = atoi(argv[1]);
		hasFilename = false;
	}

	HexPlanet planet(level, 0, 0);

	if (hasFilename)
	{
		std::ifstream is(argv[1]);
		planet.read(is);
	}

	planet.repairNormals();
	planet.write(std::cout);

	return 0;
}

