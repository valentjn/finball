#ifndef LATTICE_BOLTZMANN_HPP_
#define LATTICE_BOLTZMANN_HPP_

#include "Parameters.hpp"
#include "LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmannOutput.hpp"

class LatticeBoltzmann {
private:
    Parameters &parameters;

public:
    LatticeBoltzmann(Parameters &parameters) : parameters(parameters)
    {}

    void compute(const LatticeBoltzmannInput &input,
                 LatticeBoltzmannOutput &latticeBoltzmannOutput)
    {
        // TODO
    }
};

#endif
