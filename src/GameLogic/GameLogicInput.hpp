#ifndef GAME_LOGIC_INPUT_HPP_
#define GAME_LOGIC_INPUT_HPP_

#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"
#include "UserInput/UserInputOutput.hpp"

class GameLogicInput {
public:
    bool quit;

    GameLogicInput() {}

    GameLogicInput(const UserInputOutput &userInputOutput,
                   const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput,
                   const LatticeBoltzmannOutput &latticeBoltzmannOutput) {
        quit = userInputOutput.quit;
    }
};

#endif
