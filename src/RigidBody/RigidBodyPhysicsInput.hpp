#ifndef RIGID_BODY_PHYSICS_INPUT_HPP_
#define RIGID_BODY_PHYSICS_INPUT_HPP_

#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "UserInput/UserInputOutput.hpp"

class RigidBodyPhysicsInput {
public:
    RigidBodyPhysicsInput() {}

    RigidBodyPhysicsInput(const UserInputOutput &userInputOutput,
                          const LatticeBoltzmannOutput &latticeBoltzmannOutput) {}
};

#endif
