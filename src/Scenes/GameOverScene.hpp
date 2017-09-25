#ifndef GAME_OVER_SCENE_HPP_
#define GAME_OVER_SCENE_HPP_

#include "Highscores.hpp"
#include "Scenes/MenuRenderer.hpp"
#include "SDL/SDLWindow.hpp"

class GameOverScene {
private:
    MenuRenderer menuRenderer;
    Highscores &highscores;

public:
    GameOverScene(const SDLWindow &window, Highscores &highscores) :
            menuRenderer(window), highscores(highscores) {}

    void show(float highscore);

private:
    void listen(float highscore);
};

#endif
