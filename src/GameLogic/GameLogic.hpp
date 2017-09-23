#ifndef GAME_LOGIC_HPP_
#define GAME_LOGIC_HPP_

#include <chrono>
#include <fstream>

#include "GameLogic/GameLogicInput.hpp"
#include "GameLogic/GameLogicOutput.hpp"
#include "Highscores.hpp"
#include "Log.hpp"
#include "Visualization/RenderObject.hpp"

using namespace std;
using namespace std::chrono;

class GameLogic {
private:
    RenderObject testRenderObject;

    steady_clock::time_point startTime;

public:
    GameLogic() {
        testRenderObject.position = {0.f, 0.f, 0.f};
        testRenderObject.scale = 1.f;
        // TODO: add proper mesh!
        testRenderObject.mesh = nullptr;
        startTime = steady_clock::now();
        Log::debug("Haiscore clock started");
    }

    void update(const GameLogicInput &input, GameLogicOutput &output) {
        duration<float> duration = steady_clock::now() - startTime;
        output.highscore = duration.count();

        if (input.quit) {
            output.running = false;
            Highscores::saveHighscore(output.highscore);
            return;
        }

        if (output.objectsToRender.empty()) {
            output.objectsToRender.push_back(testRenderObject);
        }
    }
};

#endif
