#ifndef RIGID_BODY_HPP_
#define RIGID_BODY_HPP_

#include "glm/vec2.hpp"

struct RigidBody {
    int id;
    glm::vec2 position;
    bool isFixed;
    float angle;
    float radius;

    RigidBody(glm::vec2 pos, bool isFixed) : position(pos), isFixed(isFixed) {}
    RigidBody(int x, int y, bool isFixed) : RigidBody(glm::vec2(x, y), isFixed) {}
    RigidBody(int x, int y) : RigidBody(x, y, true) {}
};

#endif // RIGID_BODY_HPP_
