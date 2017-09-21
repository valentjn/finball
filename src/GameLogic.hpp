#ifndef GAME_LOGIC_HPP_
#define GAME_LOGIC_HPP_

#include <chrono>
#include <iostream>

#include "GameLogicInput.hpp"
#include "GameLogicOutput.hpp"
#include "Parameters.hpp"
#include "renderer/RenderObject.hpp"

using namespace std;
using namespace std::chrono;

class GameLogic {
private:
    Parameters &parameters;
    RenderObject testRenderObject;

    steady_clock::time_point startTime;

public:
    GameLogic(Parameters &parameters) : parameters(parameters) {
        testRenderObject.position = {0.f, 0.f, 0.f};
        testRenderObject.scale = 1.f;

        startTime = steady_clock::now();
        if (parameters.verbosity_level >= 1) {
            cout << "Highscore clock started" << endl << endl;
        }
    }

    void update(const GameLogicInput &input, GameLogicOutput &output) {
        if (input.quit) {
            output.running = false;
            return;
        }

        duration<float> duration = steady_clock::now() - startTime;
        output.highscore = static_cast<int>(duration.count());

        if (output.objectsToRender.empty()) {
            output.objectsToRender.push_back(testRenderObject);
        }
    }
};

#endif
