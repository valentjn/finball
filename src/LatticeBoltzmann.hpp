#ifndef LATTICE_BOLTZMANN_HPP_
#define LATTICE_BOLTZMANN_HPP_

#include "Parameters.hpp"
#include "Level.hpp"
#include "LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmannOutput.hpp"

class LatticeBoltzmann {
private:
    Parameters &parameters;
    Level &level;

public:
    LatticeBoltzmann(Parameters &parameters, Level &level) :
        parameters(parameters),
        level(level)
    {}

    void compute(const LatticeBoltzmannInput &input,
                 LatticeBoltzmannOutput &latticeBoltzmannOutput)
    {
        // TODO
    }
};

#endif
