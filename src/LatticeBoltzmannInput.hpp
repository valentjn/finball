#ifndef LATTICE_BOLTZMANN_INPUT_HPP_
#define LATTICE_BOLTZMANN_INPUT_HPP_

#include <memory>

#include "Array2D.hpp"
#include "RigidBodyPhysicsOutput.hpp"
#include "UserInputOutput.hpp"
#include "glm/vec3.hpp"
#include "Level.hpp"

class LatticeBoltzmannInput {
public:
    // input x, y velocity of moving bodies
    std::unique_ptr<Array2D<glm::vec2>> velocities = nullptr;

    // input flag matrix
    std::unique_ptr<Array2D<Level::CellType>> flagfield = nullptr;

    LatticeBoltzmannInput() {}

    LatticeBoltzmannInput(const UserInputOutput &userInputOutput,
                          const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput)
        : matrix(std::make_unique<Array2D<glm::vec3>>(42, 42)) {}
};

#endif
