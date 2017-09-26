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

using namespace std;

class MainMenuScene : public Scene {
public:
    unique_ptr<Scene> show();

	MainMenuScene(Scene::Context context) : Scene(context) {}

private:
    void render();
    void listen();
    string getHighscoreText();
};

#endif
