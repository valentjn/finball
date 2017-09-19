#ifndef LATTICE_BOLTZMANN_INPUT_HPP_
#define LATTICE_BOLTZMANN_INPUT_HPP_

#include "UserInputOutput.hpp"
#include "RigidBodyOutput.hpp"

class LatticeBoltzmannInput {
public:
    LatticeBoltzmannInput(){}

    LatticeBoltzmannInput(const UserInputOutput *userInputOutput,
                          const RigidBodyOutput *rigidBodyOutput)
    {}
};

#endif
