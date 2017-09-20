#ifndef GAME_LOGIC_HPP_
#define GAME_LOGIC_HPP_

#include "Parameters.hpp"
#include "GameLogicInput.hpp"
#include "GameLogicOutput.hpp"
#include "renderer/RenderObject.hpp"

class GameLogic {
private:
    Parameters &parameters;
    RenderObject testRenderObject;

public:
    GameLogic(Parameters &parameters) : parameters(parameters)
    {
        testRenderObject.position = {0.f, 0.f, 0.f};
        testRenderObject.scale = 1.f;
    }

    void update(const GameLogicInput &input,
                GameLogicOutput &output)
    {
        if (input.quit)
        {
            output.running = false;
            return;
        }

        if (output.objectsToRender.empty())
        {
            output.objectsToRender.push_back(testRenderObject);
        }
    }
};

#endif
