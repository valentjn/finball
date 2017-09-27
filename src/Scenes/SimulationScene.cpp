#include "Scenes/SimulationScene.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "DoubleBuffer.hpp"
#include "GameLogic/GameLogicInput.hpp"
#include "GameLogic/GameLogicOutput.hpp"
#include "Highscores.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "LevelDesign/Level.hpp"
#include "Log.hpp"
#include "Scenes/GameOverScene.hpp"
#include "Scenes/GameComponent.hpp"
#include "RigidBody/RigidBodyPhysicsInput.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"
#include "Timer.hpp"
#include "SDL/SDLWindow.hpp"
#include "SDL/SDLRenderer.hpp"
#include "UserInput/UserInputOutput.hpp"
#include "Visualization/RendererInput.hpp"
#include "Timer.hpp"

using namespace chrono;

std::unique_ptr<Scene> SimulationScene::show() {
    float score = simulation();

    return std::make_unique<GameOverScene>(context, score);
}

float SimulationScene::simulation()
{
	context.music->play("data/GameTheme.mp3");

    // initialize renderer before level
	GameComponent<Renderer, RendererInput, RendererOutput> renderer{ *context.window };

    LevelLoader levelLoader("data/" + levelName);
    Level level;
    levelLoader.load(level);
    renderer.getComp().setCameraTransformFromLevel(level);

	GameComponent<UserInput, UserInputInput, UserInputOutput> userInput;
	GameComponent<GameLogic, GameLogicInput, GameLogicOutput> gameLogic{ level };
	GameComponent<RigidBodyPhysics, RigidBodyPhysicsInput, RigidBodyPhysicsOutput> rigidBodyPhysics{ level };
	GameComponent<LatticeBoltzmann, LatticeBoltzmannInput, LatticeBoltzmannOutput> latticeBoltzmann{ level };

	gameLogic.bindInput(userInput, rigidBodyPhysics, latticeBoltzmann);
	rigidBodyPhysics.bindInput(userInput, latticeBoltzmann);
	latticeBoltzmann.bindInput(rigidBodyPhysics);
	renderer.bindInput(gameLogic, rigidBodyPhysics, latticeBoltzmann);
	
	const bool& running = gameLogic.getOutput().running;

	rigidBodyPhysics.run(context.parameters->simulationRate, running);
	latticeBoltzmann.run(context.parameters->simulationRate, running);
	userInput.run(context.parameters->simulationRate, running);
	gameLogic.run(context.parameters->simulationRate, running);
	renderer.run(context.parameters->frameRate, running, true);

    return gameLogic.getOutput().score;
}
