#ifndef GAME_LOGIC_HPP_
#define GAME_LOGIC_HPP_

#include <chrono>
#include <fstream>

#include "GameLogic/GameLogicInput.hpp"
#include "GameLogic/GameLogicOutput.hpp"
#include "LevelDesign/Level.hpp"
#include "Log.hpp"
#include "Visualization/RenderObject.hpp"

using namespace std;
using namespace std::chrono;

class GameLogic {
private:
    std::unique_ptr<TexturedMesh> fluid_mesh;
    RenderObject fluid_surface;

    steady_clock::time_point startTime;

public:
    GameLogic(const Level& level) {
        auto rectangle = Mesh::createRectangle({-1.f, -1.f}, {1.f, 1.f});
        fluid_mesh = std::make_unique<TexturedMesh>(rectangle, nullptr);
        fluid_surface.position = {(level.width - 1) * 0.5f, (level.height - 1) * 0.5f, 0.2f};
        fluid_surface.rotation = 0;
        fluid_surface.scale = { (level.width - 2) * 0.5f, (level.height - 2) * 0.5f };
        fluid_surface.mesh = fluid_mesh.get();

        startTime = steady_clock::now();
        Log::debug("Haiscore clock started ;-)");
    }

    void update(const GameLogicInput &input, GameLogicOutput &output) {
        duration<float> duration = steady_clock::now() - startTime;
        output.highscore = duration.count();

        for (Transform const *rigidBody : *input.rigidBodies) {
            if (rigidBody->id == 1 && rigidBody->position.y < 0) {
                output.running = false;
                return;
            }
        }

        if (input.quit) {
            output.running = false;
            return;
        }

        if (output.objectsToRender.empty()) {
            output.objectsToRender.push_back(fluid_surface);
        }

        output.fluid_mesh = fluid_mesh.get();
    }
};

#endif
