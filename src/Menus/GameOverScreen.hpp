#ifndef GAME_OVER_SCREEN_HPP_
#define GAME_OVER_SCREEN_HPP_

#include "GameScreen.hpp"
#include "Highscores.hpp"
#include "SDL/SDLWindow.hpp"

class GameOverScreen {
private:
    GameScreen gameScreen;
    Highscores &highscores;

public:
    GameOverScreen(const SDLWindow &window, Highscores &highscores) :
            gameScreen(window), highscores(highscores) {}

    void show(float highscore);

private:
    void listen(float highscore);
};

#endif
