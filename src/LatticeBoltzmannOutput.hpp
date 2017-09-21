#ifndef LATTICE_BOLTZMANN_OUTPUT_HPP_
#define LATTICE_BOLTZMANN_OUTPUT_HPP_

#include <memory>
#include <glm/vec3.hpp>

#include "Array2D.hpp"
#include "FICell.hpp"

class LatticeBoltzmannOutput {
public:
    // velocity field
    std::unique_ptr<Array2D<glm::vec2>> velocity = nullptr;
    // density field 
    std::unique_ptr<Array2D<float>> density = nullptr;
    // constants
    float c = 1./3.14;
    float dt = 0;
    float dx = 0;
    int cx [9] = {0, 1, 0, -1, 0 , 1, -1, -1, 1};
    int cy [9] = {0, 0, 1, 0, -1, 1, 1, -1, -1};
    // f_i outputs
    std::unique_ptr<Array2D<FICell>> prestream = nullptr;
    std::unique_ptr<Array2D<FICell>> afterstream = nullptr;
};

#endif
