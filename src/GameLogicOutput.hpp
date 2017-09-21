#ifndef GAME_LOGIC_OUTPUT_HPP_
#define GAME_LOGIC_OUTPUT_HPP_

#include <vector>

#include "renderer/RenderObject.hpp"

class GameLogicOutput {
public:
    bool running;
    std::vector<RenderObject> objectsToRender;

    GameLogicOutput() : running(true) {}
};

#endif
