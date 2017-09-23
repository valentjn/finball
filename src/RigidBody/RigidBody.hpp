#ifndef RIGID_BODY_HPP_
#define RIGID_BODY_HPP_

#include <glm/vec2.hpp>

struct RigidBody {
    int id;
    glm::vec2 position;
    bool isFixed;
    float angle;
    float radius;

	RigidBody(glm::vec2 pos, bool isFixed) :
			id(0), position(pos), isFixed(isFixed), angle(0.f), radius(1.f) {
	}
    RigidBody(float x, float y, bool isFixed) : RigidBody(glm::vec2(x, y), isFixed) {}
    RigidBody(float x, float y) : RigidBody(x, y, true) {}
    virtual ~RigidBody() {}
};

#endif
