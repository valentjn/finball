#ifndef RIGID_BODY_HPP_
#define RIGID_BODY_HPP_

#include <glm/vec2.hpp>

struct RigidBody {
    int id;
    glm::vec2 position;
    bool isFixed;
    float angle;
    float radius;

    RigidBody(int id, float x, float y, bool isFixed) : id(id), position(x, y), isFixed(isFixed),
        angle(0), radius(1.f) {}
    RigidBody(int id, float x, float y) : RigidBody(id, x, y, true) {}
    virtual ~RigidBody() {}
};

#endif
