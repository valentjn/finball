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

        timer.start(33, gameLogicOutput.running);

        showEndscreen(window, gameLogicOutput.highscore);
    }

private:

    void showEndscreen(SDLWindow &window, float highscore) {
        SDLRenderer renderer(window);

        int windowWidth = window.getWidth();
        int windowHeight = window.getHeight();
        int headerFontSize = windowHeight * 0.12;
        int defaultFontSize = windowHeight * 0.05;

        SDLRenderer::Font headerFont =
                renderer.loadFont("data/OpenSans-Regular.ttf", headerFontSize);
        SDLRenderer::Font defaultFont =
                renderer.loadFont("data/OpenSans-Regular.ttf", defaultFontSize);

        SDLRenderer::Color colorLight = {255, 255, 255, 0};
        SDLRenderer::Color colorDark = {50, 50, 50, 0};

        renderer.addImage("data/background.jpg");
        renderer.addText("Game Over :(", windowWidth * 0.5, windowHeight * 0.1, headerFont, colorLight, true);

        renderer.addText("This was HAIkel! Your HAIscore is " + std::to_string(highscore) + ".",
                         windowWidth * 0.5, windowHeight * 0.9, defaultFont, colorDark, true);

        renderer.addText("Enter your name:\n__________________", windowWidth * 0.1,
                         windowHeight * 0.25, defaultFont, colorLight, false);

        renderer.render();

        // Flag for quitting the program
        bool quit = false;
        SDL_Event event;

        std::string name = "";
        char c;
        SDL_Keycode sym;

        while (!quit) {
            SDL_WaitEvent(&event);

            switch (event.type) {
            case SDL_KEYDOWN:
                sym = event.key.keysym.sym;
                if (sym == SDLK_RETURN) {
                    quit = true;

                    highscores.saveHighscore(highscore, name);
                    break;
                }

                c = *SDL_GetKeyName(sym);
                Log::info("Key pressed: %c, sym: %d", c, sym);
                if (SDLK_a <= sym && sym <= SDLK_z && name.length() < 10) {
                    name += c;
                    renderer.addText("\n" + name, windowWidth * 0.1,
                                     windowHeight * 0.25, defaultFont, colorLight, false);
                    renderer.render();
                }
                break;
            case SDL_QUIT: // User hits the "x" in the corner of the window
                quit = true;
                break;
            }
        }
    }
};

#endif
