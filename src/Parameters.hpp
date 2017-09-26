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
    bool fullscreen = false;
    int windowWidth = 800;
    int windowHeight = 600;
    int frameRate = 30;
    int simulationRate = 30;
    string level = "testLevel2.txt";
    string userInputSource = "AUTO";

    Parameters(int argc, char *const argv[]) {
        int optchar;
        const char *options = "v:fw:h:r:s:l:i:";

        while ((optchar = getopt(argc, argv, options)) > 0) {
            switch (optchar) {
            case 'v':
                verbosityLevel = atoi(optarg);
                break;

            case 'f':
                fullscreen = true;
                break;

            case 'w':
                windowWidth = atoi(optarg);
                break;

            case 'h':
                windowHeight = atoi(optarg);
                break;

            case 'r':
                frameRate = atoi(optarg);
                break;

            case 's':
                simulationRate = atoi(optarg);
                break;

            case 'l':
                level = optarg;
                break;

            case 'i':
                userInputSource = optarg;
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
        cout << "	-v [level]           Verbosity level (default: 0)" << endl;
        cout << "	-f                   Enable fullscreen" << endl;
        cout << "	-w [width]           Screen height (default: 800)" << endl;
        cout << "	-h [height]          Screen width (default: 600)" << endl;
        cout << "	-r [frameRate]       Max. frame rate (default: 30)" << endl;
        cout << "	-s [simulationRate]  Simulation rate (default: 30)" << endl;
        cout << "	-l [name]            Name of the level (default: testLevel2.txt)" << endl;
        cout << "	-i [source]          input source, one of AUTO, CHOOSING, KEYBOARD";
#ifndef WITHOUT_KINECT_LIBRARIES
        cout << ", KINECT";
#endif
        cout << endl;
        exit(EXIT_FAILURE);
    }
};

#endif
