#ifndef MAIN_MENU_SCENE_HPP_
#define MAIN_MENU_SCENE_HPP_

#include <string>
#include <vector>

#include "Highscores.hpp"
#include "LevelDesign/Level.hpp"
#include "Scenes/Scene.hpp"
#include "SDL/SDLRenderer.hpp"
#include "SDL/SDLWindow.hpp"

class MainMenuScene : public Scene {
private:
    const SDLWindow &window;
    Highscores &highscores;

    // TODO refactor
    Level &level;
    int frameRate;

public:
    MainMenuScene(const SDLWindow &window, Highscores &highscores, Level &level, int frameRate) :
            window(window), highscores(highscores), level(level), frameRate(frameRate) {}

    std::unique_ptr<Scene> show();

private:
    void listen();

    std::string getHighscoreText();
};

#endif
