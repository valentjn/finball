#include <iomanip>
#include <sstream>

#include "GameController.hpp"
#include "Highscores.hpp"
#include "Level.hpp"
#include "MainMenu.hpp"
#include "SDL/SDLRenderer.hpp"
#include "SDL/SDLWindow.hpp"

std::unique_ptr<Level> MainMenu::show() {
    setup();

    // Flag for quitting the program
    bool quit = false;
    SDL_Event event;

    while (!quit) {
        SDL_WaitEvent(&event);

        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_SPACE) {
                quit = true;
            }
            break;
        case SDL_QUIT: // User hits the "x" in the corner of the window
            exit(0);
            break;
        }
    }

    return std::make_unique<Level>("data/testLevel.txt");
}

void MainMenu::setup() {
    gameScreen.addBackgroundImage("data/background.jpg");

    gameScreen.addTitle("FinBall");
    gameScreen.addActionText("To start the game please do a HAI-five or press SPACE!");
    gameScreen.addLeftText(getHighscoreText());

    gameScreen.render();
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
