#ifndef GAME_LOGIC_HPP_
#define GAME_LOGIC_HPP_

#include <chrono>
#include <fstream>

#include "GameLogic/GameLogicInput.hpp"
#include "GameLogic/GameLogicOutput.hpp"
#include "Highscores.hpp"
#include "Log.hpp"
#include "Visualization/RenderObject.hpp"
#include <Level.hpp>

using namespace std;
using namespace std::chrono;

class GameLogic {
private:
    std::unique_ptr<TexturedMesh> fluid_mesh;
    RenderObject fluid_surface;

    steady_clock::time_point startTime;

    void endGame(GameLogicOutput &output) {
        output.running = false;
        Highscores::saveHighscore(output.highscore);
        return;
    }

public:
    GameLogic(const Level& level) {
        auto rectangle = Mesh::createRectangle({-1.f, -1.f}, {1.f, 1.f});
        fluid_mesh = std::make_unique<TexturedMesh>(rectangle, nullptr);
        fluid_surface.position = {(level.width - 1) * 0.5f, (level.height - 1) * 0.5f, 0.f};
        fluid_surface.scale = level.width * 0.5f;
        fluid_surface.mesh = fluid_mesh.get();

        startTime = steady_clock::now();
        Log::debug("Haiscore clock started ;-)");
    }

    void update(const GameLogicInput &input, GameLogicOutput &output) {
        duration<float> duration = steady_clock::now() - startTime;
        output.highscore = duration.count();

        for (RigidBody const *rigidBody : *input.rigidBodies) {
            if (rigidBody->id == 1) {
                Log::debug("Ball y pos: %f", rigidBody->position.y);
                if(rigidBody->position.y < 0) {
                    endGame(output);
                    return;
                }
            }
        }

        if (input.quit) {
            endGame(output);
            return;
        }

        if (output.objectsToRender.empty()) {
            output.objectsToRender.push_back(fluid_surface);
        }

        output.fluid_mesh = fluid_mesh.get();
    }
};

#endif
