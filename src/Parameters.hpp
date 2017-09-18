#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <stdlib.h>
#include <unistd.h>
#include <iostream>

class Parameters
{
public:
	bool running;

	/**
	 * verbosity level.
	 * the higher, the more output has to be generated
	 */
	int verbosity_level;

	Parameters()
	{
		running = true;
		verbosity_level = -1;
	}

	/**
	 * this function prints the program parameters to stdout
	 */
	void printHelp(char *const argv[])
	{
		std::cout << "Parameters for " << argv[0] << std::endl;
		std::cout << "	-v [level]		Verbosity level (default: -1)" << std::endl;
		exit(EXIT_FAILURE);
	}

	/**
	 * setup the parameters given to the program.
	 *
	 * if any (optimistic!) of those parameters are invalid,
	 * printHelp is called.
	 */
	void setup(int argc, char *const argv[])
	{
		int optchar;
		const char *options = "v:";

		while ((optchar = getopt(argc, argv, options)) > 0)
		{
			switch(optchar)
			{
			case 'v':
				verbosity_level = atoi(optarg);
				break;

			default:
				printHelp(argv);
				break;
			}
		}
	}

};

#endif /* PARAMETERS_HPP_ */
