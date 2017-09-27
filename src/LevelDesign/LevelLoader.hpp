#ifndef LEVEL_LOADER_HPP_
#define LEVEL_LOADER_HPP_

#include <glm/glm.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "LevelDesign/ASCIIArtLevelParser.hpp"
#include "LevelDesign/ImageRecLevelParser.hpp"
#include "LevelDesign/Level.hpp"

using namespace std;

class LevelLoader {
public:
    static const float FLIPPER_Y, FLIPPER_WIDTH, FLIPPER_HEIGHT, FLIPPER_GAP;

private:
    ASCIIArtLevelParser asciiParser;
    ImageRecLevelParser imageParser;

public:
    LevelLoader(string filePath) : asciiParser(filePath), imageParser(filePath) {}

    void load(Level &level) {
        if (!imageParser.parse(level) && !asciiParser.parse(level)) {
            Log::error("Could not load level!");
            throw runtime_error("Could not load level");
        }
        createFlippers(level);
        debugPrint(level);
    }

private:
    void createFlippers(Level &level);

    void debugPrint(Level &level);
};

#endif
