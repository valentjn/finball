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

    void showEndscreen(float highscore) {
        gameScreen.addBackgroundImage("data/background.jpg");

        gameScreen.addTitle("Game Over :(");
        gameScreen.addActionText("This was HAIkel! Your HAIscore is " + std::to_string(highscore) + ".");
        gameScreen.addLeftText("Enter your name:\n__________________");

        gameScreen.render();

        // Flag for quitting the program
        bool quit = false;
        SDL_Event event;

        std::string name = "";
        char c;
        SDL_Keycode sym;

        while (!quit) {
            SDL_WaitEvent(&event);

            switch (event.type) {
            case SDL_KEYDOWN:
                sym = event.key.keysym.sym;
                if (sym == SDLK_RETURN) {
                    quit = true;

                    highscores.saveHighscore(highscore, name);
                    break;
                }

                c = *SDL_GetKeyName(sym);
                Log::info("Key pressed: %c, sym: %d", c, sym);
                if (SDLK_a <= sym && sym <= SDLK_z && name.length() < 10) {
                    name += c;
                    gameScreen.addLeftText("\n" + name);
                    gameScreen.render();
                }
                break;
            case SDL_QUIT: // User hits the "x" in the corner of the window
                quit = true;
                break;
            }
        }
    }
};

#endif
