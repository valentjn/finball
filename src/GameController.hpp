#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

#include "Parameters.hpp"
#include "Array2D.hpp"

#include "GameLogic.hpp"
#include "LevelLoader.hpp"
#include "UserInput.hpp"
#include "UserInputData.hpp"
#include "LatticeBoltzmann.hpp"
#include "LatticeBoltzmannData.hpp"
#include "RigidBody.hpp"
#include "RigidBodyData.hpp"
#include "Renderer.hpp"

class GameController {
private:
    Parameters &parameters;


public:
    GameController(Parameters &parameters) :
            parameters(parameters)
    {}

    void startGame(Array2D<LevelLoader::CellType> &level)
    {
        GameLogic gameLogic(parameters);
        UserInput userInput(parameters);
        LatticeBoltzmann latticeBoltzmann(parameters);
        RigidBody rigidBody(parameters);
        Renderer renderer(parameters);

        UserInputData userInputData;
        LatticeBoltzmannData latticeBoltzmannData;
        RigidBodyData rigidBodyData;
        GameLogicData gameLogicData;


        while (gameLogicData.running) {
            // 1. get user input (kinect)
            userInput.getInput(&userInputData);

            // 2. do calculations (rigid body, LBM)
            latticeBoltzmann.compute(&userInputData, &rigidBodyData, &latticeBoltzmannData);
            rigidBody.compute(&userInputData, &latticeBoltzmannData, &rigidBodyData);

            gameLogic.update(&userInputData, &rigidBodyData, &latticeBoltzmannData, &gameLogicData);

            // 3. draw visualization
            renderer.render(&latticeBoltzmannData, &rigidBodyData);
        }
    }
};

#endif
