#ifndef LATTICE_BOLTZMANN_INPUT_HPP_
#define LATTICE_BOLTZMANN_INPUT_HPP_

#include <memory>

#include <glm/vec3.hpp>

#include "Array2D.hpp"
#include "Level.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"
#include "UserInput/UserInputOutput.hpp"

class LatticeBoltzmannInput {
public:
    // input x, y velocity of moving bodies
    std::unique_ptr<Array2D<glm::vec2>> velocities = nullptr;

    // input flag matrix
    std::unique_ptr<Array2D<Level::CellType>> flagfield = nullptr;

    LatticeBoltzmannInput(Level &level) {
        velocities = std::make_unique<Array2D<glm::vec2>>(level.width, level.height);
        flagfield = std::make_unique<Array2D<Level::CellType>>(level.width, level.height);
    }

    LatticeBoltzmannInput(const UserInputOutput &userInputOutput,
                          const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput) {
        int width = rigidBodyPhysicsOutput.grid_objects.width();
        int height = rigidBodyPhysicsOutput.grid_objects.height();
        velocities = std::make_unique<Array2D<glm::vec2>>(width, height);
        flagfield = std::make_unique<Array2D<Level::CellType>>(width, height);
    }
};

#endif
