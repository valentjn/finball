#ifndef GAME_OVER_SCENE_HPP_
#define GAME_OVER_SCENE_HPP_

#include "Highscores.hpp"
#include "LevelDesign/Level.hpp"
#include "Scenes/MenuRenderer.hpp"
#include "Scenes/Scene.hpp"
#include "SDL/SDLMusic.hpp"
#include "SDL/SDLWindow.hpp"

class GameOverScene : public Scene {
private:
    float score;

public:
	GameOverScene(Scene::Params scene_params, float score) : Scene(scene_params), score(score) {}
    
	std::unique_ptr<Scene> show();

private:
    void listen(MenuRenderer &menuRenderer);
};

#endif
