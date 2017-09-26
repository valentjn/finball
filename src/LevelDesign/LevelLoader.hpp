#ifndef LEVEL_LOADER_HPP_
#define LEVEL_LOADER_HPP_

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "LevelDesign/ASCIIArtLevelParser.hpp"
#include "LevelDesign/Level.hpp"

using namespace std;

class LevelLoader {
public:
    static const int FLIPPER_Y, FLIPPER_WIDTH, FLIPPER_GAP;

private:
    ASCIIArtLevelParser levelParser;

public:
    LevelLoader(string filePath) : levelParser(filePath) {}

    void load(Level &level) {
        levelParser.parse(level);
        createFlippers(level);
        debugPrint(level);
    }

private:
    void createFlippers(Level &level);

    void debugPrint(Level &level);
};

#endif
