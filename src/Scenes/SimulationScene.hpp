#ifndef SIMULATION_SCENE_HPP_
#define SIMULATION_SCENE_HPP_

#include <memory>

#include "Highscores.hpp"
#include "LevelDesign/Level.hpp"
#include "Scenes/Scene.hpp"
#include "SDL/SDLMusic.hpp"
#include "SDL/SDLWindow.hpp"
#include "UserInput/UserInput.hpp"
#include "Visualization/Renderer.hpp"

class SimulationScene : public Scene {
	std::unique_ptr<Renderer> m_renderer;
	std::unique_ptr<Level> m_level;
    std::unique_ptr<UserInput> m_user_input;

public:
	SimulationScene(
		Scene::Params params,
		std::unique_ptr<Renderer> renderer,
		std::unique_ptr<Level> level,
		std::unique_ptr<UserInput> user_input)
		: Scene(params)
		, m_renderer(std::move(renderer))
		, m_level(std::move(level))
		, m_user_input(std::move(user_input))
	{}

    std::unique_ptr<Scene> show();
};

#endif
