#ifndef LATTICE_BOLTZMANN_OUTPUT_HPP_
#define LATTICE_BOLTZMANN_OUTPUT_HPP_

#include <memory>
#include <array>
#include <glm/vec2.hpp>

#include "Array2D.hpp"
#include "LatticeBoltzmann/FICell.hpp"
#include "LevelDesign/Level.hpp"

class LatticeBoltzmannOutput {
public:
    // velocity field
    Array2D<glm::vec2> velocity;
    // density field
    Array2D<float> density;
    // constants
    constexpr static float c = 1. / 1.732050; // 1 / sqrt 3
    constexpr static float dt = 1;
    constexpr static float dx = 1;
    // f_i outputs
    Array2D<FICell> prestream;
    Array2D<FICell> afterstream;
};

#endif
