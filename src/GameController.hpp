#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

#include <chrono>
#include <vector>
#include <fstream>

#include "GameLogic/GameLogic.hpp"
#include "GameLogic/GameLogicInput.hpp"
#include "GameLogic/GameLogicOutput.hpp"
#include "LatticeBoltzmann/LatticeBoltzmann.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "Level.hpp"
#include "Log.hpp"
#include "RigidBody/RigidBodyPhysics.hpp"
#include "RigidBody/RigidBodyPhysicsInput.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"
#include "UserInput/UserInput.hpp"
#include "UserInput/UserInputOutput.hpp"
#include "Visualization/Renderer.hpp"
#include "Visualization/RendererInput.hpp"

using namespace chrono;

class GameController {
public:
    void startGame(SDLWindow &window, Level &level) {
        GameLogic gameLogic;
        UserInput userInput;
        LatticeBoltzmann latticeBoltzmann(level);
        RigidBodyPhysics rigidBodyPhysics(level);
        Renderer renderer(window);

        UserInputOutput userInputOutput;
        LatticeBoltzmannOutput latticeBoltzmannOutput(level);
        RigidBodyPhysicsOutput rigidBodyPhysicsOutput(level);
        GameLogicOutput gameLogicOutput;

        LatticeBoltzmannInput latticeBoltzmannInput(level);
        RigidBodyPhysicsInput rigidBodyPhysicsInput;
        GameLogicInput gameLogicInput;
        RendererInput rendererInput;

        steady_clock::time_point lastFrame = steady_clock::now();
        std::vector<steady_clock::time_point> time_points(10);
        std::remove("benchmark.txt");
        fstream file;
        file.open("benchmark.txt", fstream::out | fstream::app);
        bool benchmark = true;
        if (!file.is_open()) {
            Log::error("Failed to open benchmark file!");
            benchmark = false;
        }

        while (gameLogicOutput.running) {
            // 1. get user input (kinect)
            time_points.push_back(steady_clock::now());
            userInput.getInput(userInputOutput);
            time_points.push_back(steady_clock::now());

            // 2. do calculations (rigid body, LBM)
            rigidBodyPhysicsInput = RigidBodyPhysicsInput(userInputOutput, latticeBoltzmannOutput);
            time_points.push_back(steady_clock::now());
            rigidBodyPhysics.compute(rigidBodyPhysicsInput, rigidBodyPhysicsOutput);
            time_points.push_back(steady_clock::now());

            latticeBoltzmannInput = LatticeBoltzmannInput(userInputOutput, rigidBodyPhysicsOutput);
            time_points.push_back(steady_clock::now());
            latticeBoltzmann.compute(latticeBoltzmannInput, latticeBoltzmannOutput);
            time_points.push_back(steady_clock::now());

            gameLogicInput =
                GameLogicInput(userInputOutput, rigidBodyPhysicsOutput, latticeBoltzmannOutput);
            time_points.push_back(steady_clock::now());
            gameLogic.update(gameLogicInput, gameLogicOutput);
            time_points.push_back(steady_clock::now());

            // 3. draw visualization
            rendererInput =
                RendererInput(gameLogicOutput, rigidBodyPhysicsOutput, latticeBoltzmannOutput);
            time_points.push_back(steady_clock::now());
            renderer.update(rendererInput);
            time_points.push_back(steady_clock::now());

            if (benchmark) {
                for (int i = 0; i < 9; i++) {
                    duration<float> dur = time_points[i+1] - time_points[i];
                    file << dur.count() << ";";
                }
                file << "\n";
            }
            time_points.clear();

            if (Log::logLevel >= Log::DEBUG) {
                steady_clock::time_point thisFrame = steady_clock::now();
                duration<float> duration = thisFrame - lastFrame;
                Log::debug("FPS: %f", 1 / duration.count());
                lastFrame = thisFrame;
            }
        }
    }
};

#endif
