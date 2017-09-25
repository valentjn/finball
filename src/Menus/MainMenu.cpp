#include <iomanip>
#include <sstream>

#include <SDL2/SDL.h>

#include "GameController.hpp"
#include "Highscores.hpp"
#include "Level.hpp"
#include "Menus/MainMenu.hpp"
#include "SDL/SDLEvents.hpp"
#include "SDL/SDLRenderer.hpp"
#include "SDL/SDLWindow.hpp"

std::unique_ptr<Level> MainMenu::show() {
    gameScreen.addBackgroundImage("data/background.jpg");

    gameScreen.addTitle("FinBall");
    gameScreen.addActionText("To start the game please do a HAI-five or press SPACE!");
    gameScreen.addLeftText(getHighscoreText());

    gameScreen.render();

    listen();

    return std::make_unique<Level>("data/testLevel.txt");
}

std::string MainMenu::getHighscoreText() {
    std::stringstream stream;
    int counter = 1;

    stream << std::fixed << std::setprecision(2);
    for (const auto &highscore : highscores.getHighscores()) {
        stream << "(" << counter++ << ") " << highscore.name << " " << highscore.score << "\n";
    }
    return stream.str();
}

void MainMenu::listen() {
    // Flag for quitting the program
    bool running = true;

    SDLEvents events;
    events.setListener(SDL_KEYDOWN, [&](SDL_Event &event) {
        if (event.key.keysym.sym == SDLK_SPACE) {
            running = false;
        }
    });
    events.setListener(SDL_QUIT, [&](SDL_Event &event) {
        exit(0);
    });

    events.listen(running);
}
