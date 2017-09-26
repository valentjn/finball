#ifndef MAIN_MENU_SCENE_HPP_
#define MAIN_MENU_SCENE_HPP_

#include <string>
#include <vector>

#include "Highscores.hpp"
#include "LevelDesign/Level.hpp"
#include "Scenes/Scene.hpp"
#include "SDL/SDLMusic.hpp"
#include "SDL/SDLRenderer.hpp"
#include "SDL/SDLWindow.hpp"

class MainMenuScene : public Scene {
public:
    std::unique_ptr<Scene> show();

	MainMenuScene(Scene::Params scene_params) : Scene(scene_params) {}

private:
    void listen();
    std::string getHighscoreText();
};

#endif
