#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

#include "GameLogic/GameLogic.hpp"
#include "GameLogic/GameLogicInput.hpp"
#include "GameLogic/GameLogicOutput.hpp"
#include "LatticeBoltzmann/LatticeBoltzmann.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "Level.hpp"
#include "RigidBody/RigidBodyPhysics.hpp"
#include "RigidBody/RigidBodyPhysicsInput.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"
#include "UserInput/UserInput.hpp"
#include "UserInput/UserInputOutput.hpp"
#include "Visualization/Renderer.hpp"
#include "Visualization/RendererInput.hpp"

class GameController {
public:
    void startGame(Level &level) {
        GameLogic gameLogic;
        UserInput userInput;
        LatticeBoltzmann latticeBoltzmann(level);
        RigidBodyPhysics rigidBodyPhysics(level);
        Renderer renderer;

        UserInputOutput userInputOutput;
        LatticeBoltzmannOutput latticeBoltzmannOutput(level);
        RigidBodyPhysicsOutput rigidBodyPhysicsOutput(level);
        GameLogicOutput gameLogicOutput;

        LatticeBoltzmannInput latticeBoltzmannInput;
        RigidBodyPhysicsInput rigidBodyPhysicsInput;
        GameLogicInput gameLogicInput;
        RendererInput rendererInput;

        while (gameLogicOutput.running) {
            // 1. get user input (kinect)
            userInput.getInput(userInputOutput);

            // 2. do calculations (rigid body, LBM)
            rigidBodyPhysicsInput = RigidBodyPhysicsInput(userInputOutput, latticeBoltzmannOutput);
            rigidBodyPhysics.compute(rigidBodyPhysicsInput, rigidBodyPhysicsOutput);

            latticeBoltzmannInput = LatticeBoltzmannInput(userInputOutput, rigidBodyPhysicsOutput);
            latticeBoltzmann.compute(latticeBoltzmannInput, latticeBoltzmannOutput);

            gameLogicInput =
                GameLogicInput(userInputOutput, rigidBodyPhysicsOutput, latticeBoltzmannOutput);
            gameLogic.update(gameLogicInput, gameLogicOutput);

            // 3. draw visualization
            rendererInput =
                RendererInput(gameLogicOutput, rigidBodyPhysicsOutput, latticeBoltzmannOutput);
            renderer.update(rendererInput);
        }
    }
};

#endif
