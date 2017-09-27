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
	gameLogic.run(context.parameters->frameRate, running);
	renderer.run(context.parameters->frameRate, running, true);

    /*steady_clock::time_point lastFrame = steady_clock::now();
    bool running = true;

    Timer timer([&]() {
        // 1. get user input (kinect)
        userInput.getInput(userInputOutput.writeBuffer());
        userInputOutput.swap();

        userInputOutput.lock();
        rigidBodyPhysicsOutput.lock();
        latticeBoltzmannOutput.lock();
        GameLogicInput gameLogicInput(userInputOutput.readBuffer(),
                                      rigidBodyPhysicsOutput.readBuffer(),
                                      latticeBoltzmannOutput.readBuffer());
        latticeBoltzmannOutput.unlock();
        rigidBodyPhysicsOutput.unlock();
        userInputOutput.unlock();

        gameLogic.update(gameLogicInput, gameLogicOutput.writeBuffer());
        running = gameLogicOutput.writeBuffer().running;
        gameLogicOutput.swap();

        // 3. draw visualization
        gameLogicOutput.lock();
        rigidBodyPhysicsOutput.lock();
        latticeBoltzmannOutput.lock();
        rendererInput.process(
			gameLogicOutput.readBuffer(),
            rigidBodyPhysicsOutput.readBuffer(),
            latticeBoltzmannOutput.readBuffer());
        latticeBoltzmannOutput.unlock();
        rigidBodyPhysicsOutput.unlock();
        gameLogicOutput.unlock();

        renderer.update(rendererInput);

        if (Log::logLevel >= Log::DEBUG) {
            steady_clock::time_point thisFrame = steady_clock::now();
            duration<float> duration = thisFrame - lastFrame;
            Log::debug("FPS: %f", 1 / duration.count());
            lastFrame = thisFrame;
        }
    });

    Timer simulationTimer([&]() {
        // 2. do calculations (rigid body, LBM)
        userInputOutput.lock();
        latticeBoltzmannOutput.lock();
        RigidBodyPhysicsInput rigidBodyPhysicsInput(userInputOutput.readBuffer(), latticeBoltzmannOutput.readBuffer());
        latticeBoltzmannOutput.unlock();
        userInputOutput.unlock();

        rigidBodyPhysics.compute(rigidBodyPhysicsInput, rigidBodyPhysicsOutput.writeBuffer());
        rigidBodyPhysicsOutput.swap();

        rigidBodyPhysicsOutput.lock();
        LatticeBoltzmannInput latticeBoltzmannInput(rigidBodyPhysicsOutput.readBuffer());
        rigidBodyPhysicsOutput.unlock();

        latticeBoltzmann.compute(latticeBoltzmannInput, latticeBoltzmannOutput.writeBuffer());
        latticeBoltzmannOutput.swap();
    });

    context.music->play("data/GameTheme.mp3");

    std::thread simulationThread([&]() {
        simulationTimer.start(1000 / context.parameters->simulationRate, running);
    });

    timer.start(1000 / context.parameters->frameRate, running);

    simulationThread.join();*/

    return gameLogic.getOutput().score;
}
