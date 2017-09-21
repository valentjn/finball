#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <iostream>
#include <stdlib.h>
#include <string>
#include <unistd.h>

using namespace std;

class Parameters {
public:
    /**
     * verbosity level.
     * the higher, the more output has to be generated
     */
    int verbosity_level;
    string level_file_path;

    Parameters() {
        verbosity_level = -1;
        level_file_path = "data/testLevel.txt";
    }

    /**
     * this function prints the program parameters to stdout
     */
    void printHelp(char *const argv[]) {
        std::cout << "Parameters for " << argv[0] << std::endl;
        std::cout << "	-v [level]		Verbosity level (default: -1)" << std::endl;
        std::cout << "	-l [file]		Level (default: "
                     "data/testLevel.txt)"
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    /**
     * setup the parameters given to the program.
     *
     * if any (optimistic!) of those parameters are invalid,
     * printHelp is called.
     */
    void setup(int argc, char *const argv[]) {
        int optchar;
        const char *options = "v:l:";

        while ((optchar = getopt(argc, argv, options)) > 0) {
            switch (optchar) {
            case 'v':
                verbosity_level = atoi(optarg);
                break;

            case 'l':
                level_file_path = optarg;
                break;

            default:
                printHelp(argv);
                break;
            }
        }
    }
};

#endif /* PARAMETERS_HPP_ */
