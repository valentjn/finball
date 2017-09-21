#ifndef LATTICE_BOLTZMANN_OUTPUT_HPP_
#define LATTICE_BOLTZMANN_OUTPUT_HPP_

#include <memory>
#include <glm/vec3.hpp>

#include "Array2D.hpp"
#include "FICell.hpp"

class LatticeBoltzmannOutput {
public:
    // velocity field
    Array2D<glm::vec2> velocity;
    // density field 
    Array2D<float> density;
    // constants
    constexpr static float c = 1./1.414213; // 1 / sqrt 2
    constexpr static float dt = 1;
    constexpr static float dx = 1;
    int cx [9] = {0, 1, 0, -1, 0 , 1, -1, -1, 1};
    int cy [9] = {0, 0, 1, 0, -1, 1, 1, -1, -1};
    // f_i outputs
    Array2D<FICell> prestream;
    Array2D<FICell> afterstream;
};

#endif
