#ifndef RIGID_BODY_PHYSICS_OUTPUT_HPP_
#define RIGID_BODY_PHYSICS_OUTPUT_HPP_

#include <memory>

#include <glm/vec2.hpp>

#include "Array2D.hpp"
#include "LevelDesign/Level.hpp"
#include "Transform.hpp"

class RigidBodyPhysicsOutput {
public:
    Array2D<Level::CellType> grid_objects;
    Array2D<glm::vec2> grid_velocities;
    std::vector<Transform*> rigid_bodies;
};

#endif
