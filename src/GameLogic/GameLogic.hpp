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
    Mesh fluid_mesh;
    RenderObject fluid_surface;

    steady_clock::time_point startTime;

public:
    GameLogic() {
        fluid_surface.position = {8.f, 8.f, 0.f};
        fluid_surface.scale = 1.f;
        // TODO: add proper mesh!
        fluid_mesh = createFluidMesh(16.f, 16.f);
        fluid_surface.mesh = &fluid_mesh;
        startTime = steady_clock::now();
        Log::debug("Haiscore clock started ;-)");
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
            output.objectsToRender.push_back(fluid_surface);
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
