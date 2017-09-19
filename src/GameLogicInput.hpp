#ifndef GAME_LOGIC_INPUT_HPP_
#define GAME_LOGIC_INPUT_HPP_

#include "UserInputOutput.hpp"
#include "RigidBodyOutput.hpp"
#include "LatticeBoltzmannOutput.hpp"

class GameLogicInput {
public:
    GameLogicInput() {}

    GameLogicInput(const UserInputOutput *userInputOutput,
                   const RigidBodyOutput *rigidBodyOutput,
                   const LatticeBoltzmannOutput *latticeBoltzmannOutput)
    {}
};

#endif
