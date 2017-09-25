#ifndef MAIN_MENU_HPP_
#define MAIN_MENU_HPP_

#include <string>
#include <vector>

#include "Highscores.hpp"
#include "Level.hpp"
#include "SDL/SDLRenderer.hpp"
#include "SDL/SDLWindow.hpp"

class MainMenu {
private:
    GameScreen gameScreen;
    Highscores &highscores;

public:
    MainMenu(const SDLWindow &window, Highscores &highscores) :
            gameScreen(window), highscores(highscores) {}

    std::unique_ptr<Level> show();

private:
    void setup();

    std::string getHighscoreText();
};

#endif
