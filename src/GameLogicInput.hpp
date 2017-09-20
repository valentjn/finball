#ifndef GAME_LOGIC_INPUT_HPP_
#define GAME_LOGIC_INPUT_HPP_

#include "UserInputOutput.hpp"
#include "RigidBodyPhysicsOutput.hpp"
#include "LatticeBoltzmannOutput.hpp"

class GameLogicInput {
public:
    bool quit;

    GameLogicInput() {}

    GameLogicInput(const UserInputOutput &userInputOutput,
                   const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput,
                   const LatticeBoltzmannOutput &latticeBoltzmannOutput)
    {
        quit = userInputOutput.quit;
    }
};

#endif
