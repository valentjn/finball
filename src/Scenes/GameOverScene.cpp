#include "Scenes/GameOverScene.hpp"

#include <memory>
#include <SDL2/SDL.h>

#include "Log.hpp"
#include "Scenes/MainMenuScene.hpp"
#include "Scenes/MenuRenderer.hpp"
#include "Scenes/Scene.hpp"
#include "SDL/SDLEvents.hpp"

std::unique_ptr<Scene> GameOverScene::show() {
    MenuRenderer menuRenderer(window);
    menuRenderer.addBackgroundImage("data/background.jpg");

    menuRenderer.addTitle("Game Over :(");
    menuRenderer.addActionText("This was HAIkel! Your HAIscore is " + std::to_string(score) + ".");
    menuRenderer.addLeftText("Enter your name:\n__________________");

    menuRenderer.render();

    listen(menuRenderer);

    return std::make_unique<MainMenuScene>(window, music, highscores, level, frameRate);
}

void GameOverScene::listen(MenuRenderer &menuRenderer) {
    // Flag for quitting the program
    std::string name = "";

    SDLEvents events;
    events.setListener(SDL_KEYDOWN, [&](SDL_Event &event) {
        SDL_Keycode sym = event.key.keysym.sym;
        if (sym == SDLK_RETURN) {
            highscores.saveHighscore(score, name);
            return false;
        }

        char c = *SDL_GetKeyName(sym);
        Log::info("Key pressed: %c, sym: %d", c, sym);
        if (SDLK_a <= sym && sym <= SDLK_z && name.length() < 10) {
            name += c;
            menuRenderer.addLeftText("\n" + name);
            menuRenderer.render();
        }

        return true;
    });
    events.setListener(SDL_QUIT, [&](SDL_Event &event) {
        exit(0);
        return false;
    });

    events.listen();
}
