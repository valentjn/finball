#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

#include "Parameters.hpp"
#include "Array2D.hpp"

#include "LevelLoader.hpp"
#include "GameLogic.hpp"
#include "GameLogicOutput.hpp"
#include "UserInput.hpp"
#include "UserInputOutput.hpp"
#include "LatticeBoltzmann.hpp"
#include "LatticeBoltzmannOutput.hpp"
#include "RigidBody.hpp"
#include "RigidBodyOutput.hpp"
#include "Renderer.hpp"

#include "GameLogicInput.hpp"
#include "RigidBodyInput.hpp"
#include "LatticeBoltzmannInput.hpp"
#include "RendererInput.hpp"

class GameController {
private:
    Parameters &parameters;


public:
    GameController(Parameters &parameters) :
            parameters(parameters)
    {}

    void startGame(Array2D<LevelLoader::CellType> *level)
    {
        GameLogic gameLogic(parameters);
        UserInput userInput(parameters);
        LatticeBoltzmann latticeBoltzmann(parameters);
        RigidBody rigidBody(parameters);
        Renderer renderer(parameters);

        UserInputOutput userInputOutput;
        LatticeBoltzmannOutput latticeBoltzmannOutput;
        RigidBodyOutput rigidBodyOutput;
        GameLogicOutput gameLogicOutput;

        LatticeBoltzmannInput latticeBoltzmannInput;
        RigidBodyInput rigidBodyInput;
        GameLogicInput gameLogicInput;
        RendererInput rendererInput;

        while (gameLogicOutput.running) {
            // 1. get user input (kinect)
            userInput.getInput(&userInputOutput);

            // 2. do calculations (rigid body, LBM)
            latticeBoltzmannInput = LatticeBoltzmannInput(&userInputOutput, &rigidBodyOutput);
            latticeBoltzmann.compute(&latticeBoltzmannInput, &latticeBoltzmannOutput);

            rigidBodyInput = RigidBodyInput(&userInputOutput, &latticeBoltzmannOutput);
            rigidBody.compute(&rigidBodyInput, &rigidBodyOutput);

            gameLogicInput = GameLogicInput(&userInputOutput, &rigidBodyOutput, &latticeBoltzmannOutput);
            gameLogic.update(&gameLogicInput, &gameLogicOutput);

            // 3. draw visualization
            rendererInput = RendererInput(&rigidBodyOutput, &latticeBoltzmannOutput);
            renderer.render(&rendererInput);
        }
    }
};

#endif
