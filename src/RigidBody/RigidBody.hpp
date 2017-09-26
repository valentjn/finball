#ifndef RIGID_BODY_HPP_
#define RIGID_BODY_HPP_

#include <glm/vec2.hpp>

using namespace glm;

struct RigidBody {
public:
    int id;
    vec2 position;
    float mass;
    float rotation;

    RigidBody(int id, float x, float y, float mass = 1.f, float rotation = 0.f)
        : id(id), position(x, y), mass(mass), rotation(rotation) {}
    virtual ~RigidBody() {}
};

struct RigidBodyRect : public RigidBody {
    float width;
    float height;

    RigidBodyRect(int id, float x, float y, int width, int height, float mass = 1.f,
                  float rotation = 0.f)
        : RigidBody(id, x, y, mass, rotation), width(width), height(height) {}
};

struct RigidBodyCircle : public RigidBody {
    float radius;

    RigidBodyCircle(int id, float x, float y, float radius = 1.f, float mass = 1.f,
                    float rotation = 0.f)
        : RigidBody(id, x, y, mass, rotation), radius(radius) {}
};

struct RigidBodyTriangle : public RigidBody {
    vec2 points[3];

    RigidBodyTriangle(int id, float x, float y, vec2 p1, vec2 p2, vec2 p3, float mass = 1.f,
                      float rotation = 0.f)
        : RigidBody(id, x, y, mass, rotation) {
        points[0] = p1;
        points[1] = p2;
        points[2] = p3;
    }

    RigidBodyTriangle(int id, float x, float y, vec2 p2, vec2 p3, float mass = 1.f,
                      float rotation = 0.f)
        : RigidBodyTriangle(id, x, y, vec2(0, 0), p2, p3, mass, rotation) {}
};

#endif
