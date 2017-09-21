#ifndef GAME_LOGIC_HPP_
#define GAME_LOGIC_HPP_

#include <chrono>
#include <fstream>

#include "GameLogicInput.hpp"
#include "GameLogicOutput.hpp"
#include "Highscores.hpp"
#include "Log.hpp"
#include "renderer/RenderObject.hpp"

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

private:
    void saveHighscore(float highscore) {
        fstream file;
        file.open("build/haiscores.txt", fstream::out | fstream::app);
        if (file.is_open()) {
            file << highscore << "\n";
        } else {
            Log::error("Failed to save Haiscore");
        }
    }
};

#endif
