#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <iostream>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <glm/glm.hpp>

using namespace std;

class Parameters {
public:
    int verbosityLevel = 0;
    bool fullscreen = false;
	glm::ivec2 windowResolution{ 800, 600 };
    int frameRate = 30;
    int simulationRate = 100;
    string level = "testLevel2.txt";
    string userInputSource = "AUTO";
    bool showMiniMap = true;

    Parameters(int argc, char *const argv[]) {
        int optchar;
        const char *options = "v:fw:h:r:s:l:i:m";

        while ((optchar = getopt(argc, argv, options)) > 0) {
            switch (optchar) {
            case 'v':
                verbosityLevel = atoi(optarg);
                break;

            case 'f':
                fullscreen = true;
                break;

            case 'w':
                windowResolution.x = atoi(optarg);
                break;

            case 'h':
                windowResolution.y = atoi(optarg);
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

            case 'm':
                showMiniMap = false;
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
        cout << "	-w [width]           Screen width (default: 800)" << endl;
        cout << "	-h [height]          Screen height (default: 600)" << endl;
        cout << "	-r [frameRate]       Max. frame rate (default: 30)" << endl;
        cout << "	-s [simulationRate]  Simulation rate (default: 30)" << endl;
        cout << "	-l [name]            Name of the level (default: testLevel2.txt)" << endl;
        cout << "	-i [source]          input source, one of AUTO, CHOOSING, KEYBOARD";
#ifdef KINECT_LIBRARIES
        cout << ", KINECT";
#endif
        cout << endl;
        cout << "	-m                   Hide mini map" << endl;
        exit(EXIT_FAILURE);
    }
};

#endif
