#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <iostream>
#include <stdlib.h>
#include <string>
#include <unistd.h>

using namespace std;

class Parameters {
public:
    int verbosityLevel = 0;

    Parameters(int argc, char *const argv[]) {
        int optchar;
        const char *options = "v:";

        while ((optchar = getopt(argc, argv, options)) > 0) {
            switch (optchar) {
            case 'v':
                verbosityLevel = atoi(optarg);
                break;

            default:
                printHelp(argv);
                break;
            }
        }
    }

    /**
     * this function prints the program parameters to stdout
     */
    void printHelp(char *const argv[]) {
        cout << "Parameters for " << argv[0] << endl;
        cout << "	-v [level]		Verbosity level (default: 0)" << endl;
        exit(EXIT_FAILURE);
    }
};

#endif /* PARAMETERS_HPP_ */
