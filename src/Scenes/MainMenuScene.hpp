#ifndef MAIN_MENU_SCENE_HPP_
#define MAIN_MENU_SCENE_HPP_

#include <string>
#include <vector>

#include "Highscores.hpp"
#include "Level.hpp"
#include "Scenes/MenuRenderer.hpp"
#include "SDL/SDLRenderer.hpp"
#include "SDL/SDLWindow.hpp"

class MainMenuScene {
private:
    MenuRenderer menuRenderer;
    Highscores &highscores;

public:
    MainMenuScene(const SDLWindow &window, Highscores &highscores) :
            menuRenderer(window), highscores(highscores) {}

    std::unique_ptr<Level> show();

private:
    void listen();

    std::string getHighscoreText();
};

#endif
