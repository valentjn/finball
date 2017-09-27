#include "Scenes/GameOverScene.hpp"

#include <memory>
#include <SDL2/SDL.h>

#include "Log.hpp"
#include "Scenes/MainMenuScene.hpp"
#include "Scenes/MenuRenderer.hpp"
#include "Scenes/Scene.hpp"
#include "SDL/SDLEvents.hpp"

std::unique_ptr<Scene> GameOverScene::show() {
    render();

    return std::make_unique<MainMenuScene>(context);
}

void GameOverScene::render() {
    MenuRenderer menuRenderer(*context.window);
    menuRenderer.addBackgroundImage("data/background.jpg");
    menuRenderer.addActionText("This was HAIkel! Your HAIscore is " + std::to_string(score) + ".");

    bool newHighscore = context.highscores->checkNewHighscore(score);

    if (newHighscore) {
    	menuRenderer.addTitle("New HAIscore!!!");
        menuRenderer.addLeftText("Enter your name:\n__________________");
        context.music->play("data/WinningOutro.mp3", 1);
    } else {
        menuRenderer.addTitle("Game Over :(");
        menuRenderer.addLeftText("Press any key to continue\n");
        context.music->play("data/SadGameOver.mp3", 1);
    }

    menuRenderer.render();

    listen(menuRenderer, newHighscore);
}

void GameOverScene::listen(MenuRenderer &menuRenderer, bool newHighscore) {
    // Flag for quitting the program
    std::string name = "";

    SDLEvents events;
    events.setListener(SDL_KEYDOWN, [&](SDL_Event &event) {
        if (!newHighscore) {
    		return false;
        }

        SDL_Keycode sym = event.key.keysym.sym;
        if (sym == SDLK_RETURN) {
            context.highscores->saveHighscore(score, name);
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
