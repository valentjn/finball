#include "Scenes/GameOverScene.hpp"

#include <SDL2/SDL.h>

#include "Log.hpp"
#include "SDL/SDLEvents.hpp"

void GameOverScene::show(float highscore) {
    menuRenderer.addBackgroundImage("data/background.jpg");

    menuRenderer.addTitle("Game Over :(");
    menuRenderer.addActionText("This was HAIkel! Your HAIscore is " + std::to_string(highscore) + ".");
    menuRenderer.addLeftText("Enter your name:\n__________________");

    menuRenderer.render();

    listen(highscore);
}

void GameOverScene::listen(float highscore) {
    // Flag for quitting the program
    bool running = true;
    std::string name = "";

    SDLEvents events;
    events.setListener(SDL_KEYDOWN, [&](SDL_Event &event) {
        SDL_Keycode sym = event.key.keysym.sym;
        if (sym == SDLK_RETURN) {
            running = false;

            highscores.saveHighscore(highscore, name);
            return;
        }

        char c = *SDL_GetKeyName(sym);
        Log::info("Key pressed: %c, sym: %d", c, sym);
        if (SDLK_a <= sym && sym <= SDLK_z && name.length() < 10) {
            name += c;
            menuRenderer.addLeftText("\n" + name);
            menuRenderer.render();
        }
    });
    events.setListener(SDL_QUIT, [&](SDL_Event &event) {
        exit(0);
    });

    events.listen(running);
}
