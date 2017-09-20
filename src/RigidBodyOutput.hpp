#ifndef RIGID_BODY_OUTPUT_HPP_
#define RIGID_BODY_OUTPUT_HPP_

#include <memory>
#include <glm/vec2.hpp>

#include "Array2D.hpp"

// TODO: coordinate this with other teams
const int GRID_Y = 42;
const int GRID_X = 42;

class RigidBodyOutput {
public:
    enum class type {
        INFLOW, OUTFLOW, DYNAMIC, STATIC
    };

    const unique_ptr<Array2D<type>> grid_objects;
    const unique_ptr<Array2D<glm::vec2>> grid_velocities;

    RigidBodyOutput() :
        grid_objects(std::make_unique<Array2D<type>>(GRID_X, GRID_Y)),
        grid_velocities(std::make_unique<Array2D<glm::vec2>>(GRID_X, GRID_Y))
    {}
};

#endif
