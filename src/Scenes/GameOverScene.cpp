#include "Scenes/GameOverScene.hpp"

#include <memory>
#include <SDL2/SDL.h>

#include "Log.hpp"
#include "Scenes/MainMenuScene.hpp"
#include "Scenes/MenuRenderer.hpp"
#include "Scenes/Scene.hpp"
#include "SDL/SDLEvents.hpp"

std::unique_ptr<Scene> GameOverScene::show() {
    MenuRenderer menuRenderer(*m_params.window);
    menuRenderer.addBackgroundImage("data/background.jpg");


    menuRenderer.addActionText("This was HAIkel! Your HAIscore is " + std::to_string(score) + ".");
    if(m_params.highscores->checkNewHighscore(score)){
    	menuRenderer.addTitle("New Highscore!!!");
        menuRenderer.addLeftText("Enter your name:\n__________________");
		newHighscore = true;
		m_params.music->play("data/WinningOutro.mp3", 1);
	} else {
		menuRenderer.addTitle("Game Over :(");
		menuRenderer.addLeftText("Press any key to continue\n");
		newHighscore = false;
		m_params.music->play("data/SadGameOver.mp3", 1);
	}

    menuRenderer.render();


    listen(menuRenderer);

    return std::make_unique<MainMenuScene>(m_params);
}

void GameOverScene::listen(MenuRenderer &menuRenderer) {
    // Flag for quitting the program
    std::string name = "";

    SDLEvents events;
    events.setListener(SDL_KEYDOWN, [&](SDL_Event &event) {
    	if(not newHighscore){
    		return false;
    	}
        SDL_Keycode sym = event.key.keysym.sym;
        if (sym == SDLK_RETURN) {
            m_params.highscores->saveHighscore(score, name);
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
