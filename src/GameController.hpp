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

class GameController {
    Highscores &highscores;
    int frameTime;

public:
    GameController(Highscores &highscores, int frameRate) : highscores(highscores), frameTime(1000 / frameRate) {}

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
        	auto time2 = std::chrono::steady_clock::now();

        	userInput.getInput(userInputOutput);

            auto time1 = std::chrono::steady_clock::now();
            auto inpTime = time1 - time2;

            // 2. do calculations (rigid body, LBM)
            rigidBodyPhysicsInput = RigidBodyPhysicsInput(userInputOutput, latticeBoltzmannOutput);
            rigidBodyPhysics.compute(rigidBodyPhysicsInput, rigidBodyPhysicsOutput);

            time2=std::chrono::steady_clock::now();
			auto rbTime = time2 - time1;

            latticeBoltzmannInput = LatticeBoltzmannInput(userInputOutput, rigidBodyPhysicsOutput);
            latticeBoltzmann.compute(latticeBoltzmannInput, latticeBoltzmannOutput);

			time1=std::chrono::steady_clock::now();
			auto lbTime = time1 - time2;

			gameLogicInput =
                GameLogicInput(userInputOutput, rigidBodyPhysicsOutput, latticeBoltzmannOutput);
            gameLogic.update(gameLogicInput, gameLogicOutput);

            time2=std::chrono::steady_clock::now();
			auto glTime = time2 - time1;

			// 3. draw visualization
            rendererInput =
                RendererInput(gameLogicOutput, rigidBodyPhysicsOutput, latticeBoltzmannOutput);
            renderer.update(rendererInput);

			time1=std::chrono::steady_clock::now();
			auto visuTime = time1 - time2;

            if (Log::logLevel >= Log::DEBUG) {
                steady_clock::time_point thisFrame = steady_clock::now();
                duration<float> duration = thisFrame - lastFrame;
                Log::debug("FPS: %f", 1 / duration.count());
                lastFrame = thisFrame;
                auto gesT = inpTime + rbTime + lbTime + glTime + visuTime;
				Log::debug("Time ratio(%): inp %f, rb %f, lb %f, gl %f, visu %f", inpTime*100./gesT,
						rbTime*100./gesT, lbTime*100./gesT, glTime*100./gesT, visuTime*100./gesT);
            }
        });

        timer.start(frameTime, gameLogicOutput.running);

        GameOverScene gameOverScene(window, highscores);
        gameOverScene.show(gameLogicOutput.highscore);
    }

};

#endif
