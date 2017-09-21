#ifndef LATTICE_BOLTZMANN_OUTPUT_HPP_
#define LATTICE_BOLTZMANN_OUTPUT_HPP_

#include <memory>

#include "Array2D.hpp"
#include "glm/vec3.hpp"

class LatticeBoltzmannOutput {
public:
    // processed output field (the first two components give the velocity the
    // third one the density)
    std::unique_ptr<Array2D<glm::vec3>> matrix = nullptr;
};

#endif
