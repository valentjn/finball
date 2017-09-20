#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

#include "Level.hpp"
#include "Parameters.hpp"

#include "GameLogic.hpp"
#include "GameLogicInput.hpp"
#include "GameLogicOutput.hpp"
#include "LatticeBoltzmann.hpp"
#include "LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmannOutput.hpp"
#include "LevelLoader.hpp"
#include "RigidBodyPhysics.hpp"
#include "RigidBodyPhysicsInput.hpp"
#include "RigidBodyPhysicsOutput.hpp"
#include "UserInput.hpp"
#include "UserInputOutput.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/RendererInput.hpp"

class GameController {
  private:
    Parameters &parameters;

  public:
    GameController(Parameters &parameters) : parameters(parameters) {}

    void startGame(Level &level) {
        GameLogic gameLogic(parameters);
        UserInput userInput(parameters);
        LatticeBoltzmann latticeBoltzmann(parameters, level);
        RigidBodyPhysics rigidBodyPhysics(parameters, level);
        Renderer renderer(parameters);

        UserInputOutput userInputOutput;
        LatticeBoltzmannOutput latticeBoltzmannOutput;
        RigidBodyPhysicsOutput rigidBodyPhysicsOutput;
        GameLogicOutput gameLogicOutput;

        LatticeBoltzmannInput latticeBoltzmannInput;
        RigidBodyPhysicsInput rigidBodyPhysicsInput;
        GameLogicInput gameLogicInput;
        RendererInput rendererInput;

        while (gameLogicOutput.running) {
            // 1. get user input (kinect)
            userInput.getInput(userInputOutput);

            // 2. do calculations (rigid body, LBM)
            latticeBoltzmannInput =
                LatticeBoltzmannInput(userInputOutput, rigidBodyPhysicsOutput);
            latticeBoltzmann.compute(latticeBoltzmannInput,
                                     latticeBoltzmannOutput);

            rigidBodyPhysicsInput =
                RigidBodyPhysicsInput(userInputOutput, latticeBoltzmannOutput);
            rigidBodyPhysics.compute(rigidBodyPhysicsInput,
                                     rigidBodyPhysicsOutput);

            gameLogicInput =
                GameLogicInput(userInputOutput, rigidBodyPhysicsOutput,
                               latticeBoltzmannOutput);
            gameLogic.update(gameLogicInput, gameLogicOutput);

            // 3. draw visualization
            rendererInput =
                RendererInput(gameLogicOutput, rigidBodyPhysicsOutput,
                              latticeBoltzmannOutput);
            renderer.update(rendererInput);
        }
    }
};

#endif
