#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

#include <chrono>

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
#include "Timer.hpp"
#include "UserInput/UserInput.hpp"
#include "UserInput/UserInputOutput.hpp"
#include "Visualization/Renderer.hpp"
#include "Visualization/RendererInput.hpp"

using namespace chrono;

class GameController {

private:
    // Game components
    GameLogic gameLogic;
    UserInput userInput;
    LatticeBoltzmann latticeBoltzmann;
    RigidBodyPhysics rigidBodyPhysics;
    Renderer renderer;

    // Input objects
    LatticeBoltzmannInput latticeBoltzmannInput;
    RigidBodyPhysicsInput rigidBodyPhysicsInput;
    GameLogicInput gameLogicInput;
    RendererInput rendererInput;

    // Output objects
    UserInputOutput userInputOutput;
    LatticeBoltzmannOutput latticeBoltzmannOutput;
    RigidBodyPhysicsOutput rigidBodyPhysicsOutput;
    GameLogicOutput gameLogicOutput;

    // Mutex

public:
    GameController(SDLWindow &window, Level &level) :
        latticeBoltzmann(level), rigidBodyPhysics(level), renderer(window),
        latticeBoltzmannOutput(level), rigidBodyPhysicsOutput(level),
        latticeBoltzmannInput(level) {}

    void startGame() {
        Timer simulationTimer([this]() {
            this->simulationStep();
        });
        simulationTimer.start(100);

        Timer visualizationTimer([this]() {
            this->visualizationStep();
        });
        visualizationTimer.start(100);

        steady_clock::time_point lastFrame = steady_clock::now();

        while (gameLogicOutput.running) {
            // 1. get user input (kinect)
            // userInput.getInput(userInputOutput);

            // 2. do calculations (rigid body, LBM)
            // rigidBodyPhysicsInput = RigidBodyPhysicsInput(userInputOutput, latticeBoltzmannOutput);
            // rigidBodyPhysics.compute(rigidBodyPhysicsInput, rigidBodyPhysicsOutput);
            //
            // latticeBoltzmannInput = LatticeBoltzmannInput(userInputOutput, rigidBodyPhysicsOutput);
            // latticeBoltzmann.compute(latticeBoltzmannInput, latticeBoltzmannOutput);

            gameLogicInput =
                GameLogicInput(userInputOutput, rigidBodyPhysicsOutput, latticeBoltzmannOutput);
            gameLogic.update(gameLogicInput, gameLogicOutput);

            // 3. draw visualization
            rendererInput =
                RendererInput(gameLogicOutput, rigidBodyPhysicsOutput, latticeBoltzmannOutput);
            renderer.update(rendererInput);

            if (Log::logLevel >= Log::DEBUG) {
                steady_clock::time_point thisFrame = steady_clock::now();
                duration<float> duration = thisFrame - lastFrame;
                Log::debug("FPS: %f", 1 / duration.count());
                lastFrame = thisFrame;
            }
        }
    }
private:
    void simulationStep() {
        rigidBodyPhysicsInput = RigidBodyPhysicsInput(userInputOutput, latticeBoltzmannOutput);
        rigidBodyPhysics.compute(rigidBodyPhysicsInput, rigidBodyPhysicsOutput);

        latticeBoltzmannInput = LatticeBoltzmannInput(userInputOutput, rigidBodyPhysicsOutput);
        latticeBoltzmann.compute(latticeBoltzmannInput, latticeBoltzmannOutput);
    }

    void visualizationStep() {
        rendererInput =
            RendererInput(gameLogicOutput, rigidBodyPhysicsOutput, latticeBoltzmannOutput);
        renderer.update(rendererInput);
    }
};

#endif
