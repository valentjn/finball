#include "Scenes/SimulationScene.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "DoubleBuffer.hpp"
#include "GameLogic/GameLogicInput.hpp"
#include "GameLogic/GameLogicOutput.hpp"
#include "Highscores.hpp"
#include "Physics/Physics.hpp"
#include "LevelDesign/Level.hpp"
#include "Log.hpp"
#include "Scenes/GameOverScene.hpp"
#include "Scenes/GameComponent.hpp"
#include "Timer.hpp"
#include "SDL/SDLWindow.hpp"
#include "SDL/SDLRenderer.hpp"
#include "UserInput/UserInputOutput.hpp"
#include "Visualization/RendererInput.hpp"
#include "Timer.hpp"

using namespace chrono;

std::unique_ptr<Scene> SimulationScene::show() {
    int score = simulation();

    return std::make_unique<GameOverScene>(context, score);
}

int SimulationScene::simulation()
{
	context.music->play("data/GameTheme.mp3");

    // initialize renderer before level
	GameComponent<Renderer, RendererInput, void> renderer{ "Renderer", *context.window };

    LevelLoader levelLoader("data/" + levelName);
    Level level;
    levelLoader.load(level);
    renderer.getComp().setCameraTransformFromLevel(level);

	GameComponent<Physics, PhysicsInput, PhysicsOutput> physics{ "Physics", level };
	GameComponent<GameLogic, GameLogicInput, GameLogicOutput> gameLogic{ "GameLogic", level, context.parameters->showMiniMap };

	physics.bindInput(*userInput);
	gameLogic.bindInput(*userInput, physics);
	renderer.bindInput(gameLogic, physics);

	const bool &running = gameLogic.getOutput().running;

	physics.run(context.parameters->simulationRate, running);
	userInput->run(context.parameters->simulationRate, running);
	gameLogic.run(context.parameters->simulationRate, running);
	renderer.run(context.parameters->frameRate, running, true);

    return gameLogic.getOutput().score;
}
