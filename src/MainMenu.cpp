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
        while (SDL_PollEvent(&event)) {
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
    }

    return std::make_unique<Level>("data/testLevel.txt");
}

void MainMenu::setup() {
    int windowWidth = window.getWidth();
    int windowHeight = window.getHeight();

    // Set background
    renderer.addImage("data/background.jpg");

    renderer.addText("FinBall", windowWidth * 0.5, windowHeight * 0.1, headerFont, colorLight, true);
    renderer.addText("To start the game please do a HAI-five or press SPACE!",
                     windowWidth * 0.5, windowHeight * 0.9, defaultFont, colorDark, true);

    renderer.addText(getHighscoreText(), windowWidth * 0.1, windowHeight * 0.25, defaultFont, colorLight, false);

    renderer.render();
}

std::string MainMenu::getHighscoreText() {
    std::vector<float> highscores;
    Highscores::loadHighscores(highscores);

    std::stringstream stream;
    int counter = 1;

    stream << std::fixed << std::setprecision(2);
    for (auto it = highscores.begin(); it != highscores.end() && counter <= 5; it++) {
        stream << "(" << counter++ << ") " << *it << "\n";
    }
    return stream.str();
}
