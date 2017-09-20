#ifndef GAME_LOGIC_INPUT_HPP_
#define GAME_LOGIC_INPUT_HPP_

#include "LatticeBoltzmannOutput.hpp"
#include "RigidBodyPhysicsOutput.hpp"
#include "UserInputOutput.hpp"

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
