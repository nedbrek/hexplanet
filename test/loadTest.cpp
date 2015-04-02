#include "../src/hexplanet.h"
#include <cstdlib>
#include <fstream>

void printUsage()
{
	std::cerr << "Usage: loadTest.exe [-l <level>]|[<filename>]" << std::endl;
	exit(1);
}

int main(int argc, char **argv)
{
	int level = 0;
	if (argc < 2)
		printUsage();

	bool hasFilename = true;
	if (argv[1][0] == '-')
	{
		if (argc < 3)
			printUsage();

		level = atoi(argv[2]);
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

