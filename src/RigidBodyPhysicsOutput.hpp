#ifndef RIGID_BODY_PHYSICS_OUTPUT_HPP_
#define RIGID_BODY_PHYSICS_OUTPUT_HPP_

#include <glm/vec2.hpp>
#include <memory>

#include "Array2D.hpp"

// TODO: coordinate this with other teams
const int GRID_Y = 42;
const int GRID_X = 42;

class Transformation {
public:
    int id;
    glm::vec2 position;
    float angle;
};

class RigidBodyPhysicsOutput {
public:
    enum class type { INFLOW, OUTFLOW, DYNAMIC, STATIC };

    const std::unique_ptr<Array2D<type>> grid_objects;
    const std::unique_ptr<Array2D<glm::vec2>> grid_velocities;

    const std::unique_ptr<std::vector<Transformation>> transformations;

    RigidBodyPhysicsOutput()
        : grid_objects(std::make_unique<Array2D<type>>(GRID_X, GRID_Y)),
          grid_velocities(std::make_unique<Array2D<glm::vec2>>(GRID_X, GRID_Y)) {}
};

#endif
