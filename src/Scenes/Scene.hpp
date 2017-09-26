#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <memory>
#include <SDL/SDLWindow.hpp>
#include <SDL/SDLMusic.hpp>
#include <Highscores.hpp>
#include <LevelDesign/Level.hpp>
#include <Parameters.hpp>

class Scene
{
public:
	struct Params
	{
		SDLWindow* window;
		SDLMusic* music;
		Highscores* highscores;
		const Parameters* cmd_params;
	};

	Scene(Params scene_params) : m_params(scene_params) {}

	virtual std::unique_ptr<Scene> show() = 0;
	virtual ~Scene() = 0;

protected:
	Params m_params;
};

inline Scene::~Scene() {}

#endif
