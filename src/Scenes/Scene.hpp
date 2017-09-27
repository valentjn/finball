#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <memory>
#include <SDL/SDLWindow.hpp>
#include <SDL/SDLMusic.hpp>
#include <Highscores.hpp>
#include <LevelDesign/Level.hpp>
#include <Parameters.hpp>

class Scene {

public:
	struct Context {
		SDLWindow *window;
		SDLMusic *music;
		Highscores *highscores;
		const Parameters *parameters;
	};

	Scene(Context context) : context(context) {}

	virtual std::unique_ptr<Scene> show() = 0;
	virtual ~Scene() = 0;

protected:
	Context context;
};

inline Scene::~Scene() {}

#endif
