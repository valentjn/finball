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
    int score;

public:
	GameOverScene(Scene::Context context, int score) : Scene(context), score(score) {}

	std::unique_ptr<Scene> show();

private:
    void render();
    void listen(MenuRenderer &menuRenderer, bool newHighscore);
};

#endif
