#include "Scenes/SimulationScene.hpp"

#include <chrono>
#include <memory>
#include <string>

#include "GameLogic/GameLogic.hpp"
#include "GameLogic/GameLogicInput.hpp"
#include "GameLogic/GameLogicOutput.hpp"
#include "Highscores.hpp"
#include "LatticeBoltzmann/LatticeBoltzmann.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "LevelDesign/Level.hpp"
#include "Log.hpp"
#include "Scenes/GameOverScene.hpp"
#include "RigidBody/RigidBodyPhysics.hpp"
#include "RigidBody/RigidBodyPhysicsInput.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"
#include "Timer.hpp"
#include "SDL/SDLWindow.hpp"
#include "SDL/SDLRenderer.hpp"
#include "UserInput/UserInput.hpp"
#include "UserInput/UserInputOutput.hpp"
#include "Visualization/Renderer.hpp"
#include "Visualization/RendererInput.hpp"
#include "Timer.hpp"

using namespace chrono;

std::unique_ptr<Scene> SimulationScene::show() {
    Renderer renderer(window);
    GameLogic gameLogic(level);
    LatticeBoltzmann latticeBoltzmann(level);
    RigidBodyPhysics rigidBodyPhysics(level);

    UserInputOutput userInputOutput;
    LatticeBoltzmannOutput latticeBoltzmannOutput(level);
    RigidBodyPhysicsOutput rigidBodyPhysicsOutput(level);
    GameLogicOutput gameLogicOutput;

    LatticeBoltzmannInput latticeBoltzmannInput(level);
    RigidBodyPhysicsInput rigidBodyPhysicsInput;
    GameLogicInput gameLogicInput;
    RendererInput rendererInput;

    steady_clock::time_point lastFrame = steady_clock::now();

    Timer timer([&]() {
        // 1. get user input (kinect)
        userInput->getInput(userInputOutput);

        // 2. do calculations (rigid body, LBM)
        rigidBodyPhysicsInput = RigidBodyPhysicsInput(userInputOutput, latticeBoltzmannOutput);
        rigidBodyPhysics.compute(rigidBodyPhysicsInput, rigidBodyPhysicsOutput);

        latticeBoltzmannInput = LatticeBoltzmannInput(rigidBodyPhysicsOutput);
        latticeBoltzmann.compute(latticeBoltzmannInput, latticeBoltzmannOutput);

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
    });

    music.load("data/GameTheme.mp3");

    timer.start(1000 / frameRate, gameLogicOutput.running);

    return std::make_unique<GameOverScene>(window, music, highscores, gameLogicOutput.highscore, level, frameRate);
}
