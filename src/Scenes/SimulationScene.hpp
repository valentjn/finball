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
    Level level;

    unique_ptr<GameLogic> gameLogic;
    unique_ptr<UserInput> userInput;
    unique_ptr<LatticeBoltzmann> latticeBoltzmann;
    unique_ptr<RigidBodyPhysics> rigidBodyPhysics;
    unique_ptr<Renderer> renderer;

public:
    SimulationScene(Scene::Context context, string levelName) : Scene(context) {
        renderer = make_unique<Renderer>(*context.window);

        LevelLoader levelLoader("data/" + levelName);
        levelLoader.load(level);

        gameLogic = make_unique<GameLogic>(level);
        userInput = make_unique<UserInput>();
        latticeBoltzmann = make_unique<LatticeBoltzmann>(level);
        rigidBodyPhysics = make_unique<RigidBodyPhysics>(level);
    }

    std::unique_ptr<Scene> show();

};

#endif
