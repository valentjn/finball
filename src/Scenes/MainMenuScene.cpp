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
#include "LevelDesign/LevelLoader.hpp"
#include "LevelDesign/Level.hpp"

using namespace std;

unique_ptr<Scene> MainMenuScene::show() {
    render();

    // switch to simulation scene
    return std::make_unique<SimulationScene>(context, context.parameters->level);
}

void MainMenuScene::render() {
    MenuRenderer menuRenderer(*context.window);
    menuRenderer.addBackgroundImage("data/background.jpg");
    menuRenderer.addTitle("FinBall");
    menuRenderer.addActionText("To start the game please do a HAIfive or press SPACE!");
    menuRenderer.addLeftText(getHighscoreText());
    menuRenderer.render();

    context.music->play("data/MainTheme.mp3");

    listen();
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

std::string MainMenuScene::getHighscoreText() {
    std::stringstream stream;
    int counter = 1;

    stream << std::fixed << std::setprecision(2);
    for (const auto &highscore : context.highscores->getHighscores()) {
        stream << "(" << counter++ << ") " << highscore.name << " " << highscore.score << "\n";
    }

    return stream.str();
}
