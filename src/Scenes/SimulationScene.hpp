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
#include "Scenes/Scene.hpp"
#include "SDL/SDLMusic.hpp"
#include "SDL/SDLWindow.hpp"
#include "UserInput/UserInput.hpp"
#include "Visualization/Renderer.hpp"

using namespace std;

class SimulationScene : public Scene {

private:
    string levelName;

    float simulation();

public:
    SimulationScene(Scene::Context context, string levelName) :
            Scene(context), levelName(levelName) {}

    std::unique_ptr<Scene> show();
}; 

private:
    float simulation();

};

#endif
