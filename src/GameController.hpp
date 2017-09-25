#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

#include <chrono>
#include <string>

#include "GameLogic/GameLogic.hpp"
#include "GameLogic/GameLogicInput.hpp"
#include "GameLogic/GameLogicOutput.hpp"
#include "Highscores.hpp"
#include "LatticeBoltzmann/LatticeBoltzmann.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "Level.hpp"
#include "Log.hpp"
#include "Menus/GameOverScreen.hpp"
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

class GameController {

    Highscores &highscores;

public:
    GameController(Highscores &highscores) : highscores(highscores) {}

    void startGame(SDLWindow &window, Level &level) {
        Renderer renderer(window);
        GameLogic gameLogic(level);
        UserInput userInput;
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
            userInput.getInput(userInputOutput);

            // 2. do calculations (rigid body, LBM)
            rigidBodyPhysicsInput = RigidBodyPhysicsInput(userInputOutput, latticeBoltzmannOutput);
            rigidBodyPhysics.compute(rigidBodyPhysicsInput, rigidBodyPhysicsOutput);

            latticeBoltzmannInput = LatticeBoltzmannInput(userInputOutput, rigidBodyPhysicsOutput);
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

        timer.start(100, gameLogicOutput.running);

        GameOverScreen gameOverScreen(window, highscores);
        gameOverScreen.show(gameLogicOutput.highscore);
    }

};

#endif
