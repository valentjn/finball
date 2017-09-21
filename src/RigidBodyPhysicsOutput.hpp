#ifndef RIGID_BODY_PHYSICS_OUTPUT_HPP_
#define RIGID_BODY_PHYSICS_OUTPUT_HPP_

#include <glm/vec2.hpp>
#include <memory>

#include "Level.hpp"
#include "Array2D.hpp"

// TODO: coordinate this with other teams
const int GRID_Y = 42;
const int GRID_X = 42;

class RigidBodyPhysicsOutput {
public:
    Array2D<Level::CellType> grid_objects;
    Array2D<glm::vec2> grid_velocities;
    std::vector<RigidBody> rigid_bodies;

    RigidBodyPhysicsOutput(Level const &level)
        : grid_objects(level.width, level.height),
          grid_velocities(level.width, level.height) {}
};

#endif
