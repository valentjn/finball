#ifndef LATTICE_BOLTZMANN_INPUT_HPP_
#define LATTICE_BOLTZMANN_INPUT_HPP_

#include <memory>

#include <glm/vec3.hpp>

#include "Array2D.hpp"
#include "LevelDesign/Level.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"
#include "UserInput/UserInputOutput.hpp"

class LatticeBoltzmannInput {
public:
	// TODO: Both should be const
    // input x, y velocity of moving bodies
    Array2D<glm::vec2> velocities;

    // input flag matrix
    Array2D<Level::CellType> flagfield;

	void process(const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput)
	{
        velocities = rigidBodyPhysicsOutput.grid_velocities;
		flagfield = rigidBodyPhysicsOutput.grid_objects;
	}
};

#endif
