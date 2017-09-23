#ifndef RIGID_BODY_PHYSICS_OUTPUT_HPP_
#define RIGID_BODY_PHYSICS_OUTPUT_HPP_

#include <glm/vec2.hpp>
#include <memory>

#include "Array2D.hpp"
#include "Level.hpp"

class RigidBodyPhysicsOutput {
public:
    Array2D<Level::CellType> grid_objects;
    Array2D<glm::vec2> grid_velocities;
    std::vector<RigidBody> rigid_bodies;

    RigidBodyPhysicsOutput(Level const &level)
        : grid_objects(level.width, level.height), grid_velocities(level.width, level.height) {}
};

#endif
