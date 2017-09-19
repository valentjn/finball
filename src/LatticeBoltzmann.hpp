#ifndef LATTICE_BOLTZMANN_HPP_
#define LATTICE_BOLTZMANN_HPP_

#include "Parameters.hpp"
#include "UserInputData.hpp"
#include "RigidBodyData.hpp"
#include "LatticeBoltzmannData.hpp"

class LatticeBoltzmann {
private:
    Parameters &parameters;

public:
    LatticeBoltzmann(Parameters &parameters) : parameters(parameters)
    {}

    void compute(const UserInputData *userInputData,
                 const RigidBodyData *rigidBodyData,
                 LatticeBoltzmannData *latticeBoltzmannData)
    {
        // TODO
    }
};

#endif
