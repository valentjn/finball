#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

#include "Parameters.hpp"
#include "UserInput.hpp"
#include "UserInputData.hpp"
#include "LatticeBoltzmann.hpp"
#include "LatticeBoltzmannData.hpp"
#include "RigidBody.hpp"
#include "RigidBodyData.hpp"
#include "Renderer.hpp"

class GameController {
private:
    Parameters parameters;

    UserInput userInput;
    LatticeBoltzmann latticeBoltzmann;
    RigidBody rigidBody;
    Renderer renderer;


public:
    GameController(Parameters parameters) :
            parameters(parameters),
            userInput(parameters),
            latticeBoltzmann(parameters),
            rigidBody(parameters),
            renderer(parameters)
    {}

    void startGame()
    {
        UserInputData userInputData;
        LatticeBoltzmannData latticeBoltzmannData;
        RigidBodyData rigidBodyData;

        while (parameters.running) {
            // 1. get user input (kinect)
            userInput.getInput(&userInputData);

            // 2. do calculations (rigid body, LBM)
            latticeBoltzmann.compute(&userInputData, &rigidBodyData, &latticeBoltzmannData);
            rigidBody.compute(&userInputData, &latticeBoltzmannData, &rigidBodyData);

            // 3. draw visualization
            renderer.render(&latticeBoltzmannData, &rigidBodyData);
        }
    }
};

#endif
