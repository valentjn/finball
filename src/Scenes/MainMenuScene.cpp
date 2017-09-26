#include <iomanip>
#include <sstream>

#include <memory>
#include <SDL2/SDL.h>

#include "Highscores.hpp"
#include "Scenes/MainMenuScene.hpp"
#include "Scenes/MenuRenderer.hpp"
#include "Scenes/Scene.hpp"
#include "Scenes/SimulationScene.hpp"
#include "SDL/SDLEvents.hpp"
#include "SDL/SDLRenderer.hpp"
#include "SDL/SDLWindow.hpp"
#include "UserInput/UserInput.hpp"
#include "UserInput/UserInputOutput.hpp"

std::unique_ptr<Scene> MainMenuScene::show() {
    MenuRenderer menuRenderer(window);
    menuRenderer.addBackgroundImage("data/background.jpg");

    menuRenderer.addTitle("FinBall");
    menuRenderer.addActionText("To start the game please do a HAI-five or press SPACE!");
    menuRenderer.addLeftText(getHighscoreText());

    menuRenderer.render();

    music.load("data/MainTheme.mp3");

    // listen();

    auto userInput = std::make_unique<UserInput>();
    UserInputOutput userInputOutput;

    while (true) {
        userInput->getInput(userInputOutput);

        if (userInputOutput.start) {
            break;
        }
    }

    return std::make_unique<SimulationScene>(window, music, std::move(userInput), level, highscores, frameRate);
}

std::string MainMenuScene::getHighscoreText() {
    std::stringstream stream;
    int counter = 1;

    stream << std::fixed << std::setprecision(2);
    for (const auto &highscore : highscores.getHighscores()) {
        stream << "(" << counter++ << ") " << highscore.name << " " << highscore.score << "\n";
    }
    return stream.str();
}

void MainMenuScene::listen() {
    // Flag for quitting the program
    SDLEvents events;
    events.setListener(SDL_KEYDOWN, [&](SDL_Event &event) {
        if (event.key.keysym.sym == SDLK_SPACE) {
            return false;
        }

        return true;
    });
    events.setListener(SDL_QUIT, [&](SDL_Event &event) {
        exit(0);
        return false;
    });

    events.listen();
}
