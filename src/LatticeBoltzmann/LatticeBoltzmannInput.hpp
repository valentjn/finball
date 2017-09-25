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
	// TODO: Both should be const
    // input x, y velocity of moving bodies
    Array2D<glm::vec2> *velocities = nullptr;

    // input flag matrix
    Array2D<Level::CellType> *flagfield = nullptr;

    LatticeBoltzmannInput(Level &level) {}

	// TODO: const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput
    LatticeBoltzmannInput(const UserInputOutput &userInputOutput,
                          RigidBodyPhysicsOutput &rigidBodyPhysicsOutput) {
        velocities = &rigidBodyPhysicsOutput.grid_velocities;
        flagfield = &rigidBodyPhysicsOutput.grid_objects;
    }
};

#endif
