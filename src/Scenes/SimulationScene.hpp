#ifndef SIMULATION_SCENE_HPP_
#define SIMULATION_SCENE_HPP_

#include <memory>
#include <string>

#include "GameLogic/GameLogic.hpp"
#include "Highscores.hpp"
#include "LatticeBoltzmann/LatticeBoltzmann.hpp"
#include "LevelDesign/Level.hpp"
#include "LevelDesign/LevelLoader.hpp"
#include "RigidBody/RigidBodyPhysics.hpp"
#include "Scenes/GameComponent.hpp"
#include "Scenes/Scene.hpp"
#include "SDL/SDLMusic.hpp"
#include "SDL/SDLWindow.hpp"
#include "UserInput/UserInput.hpp"
#include "Visualization/Renderer.hpp"

using namespace std;

class SimulationScene : public Scene
{
private:
    string levelName;
    unique_ptr<GameComponent<UserInput, void, UserInputOutput>> userInput;

    float simulation();

public:
    SimulationScene(Scene::Context context, string levelName,
            unique_ptr<GameComponent<UserInput, void, UserInputOutput>> userInput)
		: Scene(context), levelName(levelName), userInput(move(userInput))
	{}

    std::unique_ptr<Scene> show();
};

#endif
