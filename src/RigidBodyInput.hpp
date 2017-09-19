#ifndef RIGID_BODY_INPUT_HPP_
#define RIGID_BODY_INPUT_HPP_

#include "UserInputOutput.hpp"
#include "LatticeBoltzmannOutput.hpp"

class RigidBodyInput {
public:
    RigidBodyInput() {}

    RigidBodyInput(const UserInputOutput &userInputOutput,
                   const LatticeBoltzmannOutput &latticeBoltzmannOutput)
    {}
};

#endif
