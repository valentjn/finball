#ifndef RIGID_BODY_HPP_
#define RIGID_BODY_HPP_

#include <glm/vec2.hpp>
#include <memory>
#include <vector>

#include "Visualization/Mesh.hpp"

using namespace glm;
using namespace std;

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

    RigidBodyRect(int id, float x, float y, float width, float height, float mass = 1.f,
                  float rotation = 0.f)
        : RigidBody(id, x, y, mass, rotation), width(width), height(height) {}

    unique_ptr<ColoredMesh> createColoredMesh(vec3 color=vec3(0, 0, 255)) {
        vec2 sizeHalf(width / 2.f, height / 2.f);
        return make_unique<ColoredMesh>(
            Mesh::createRectangle(-sizeHalf, sizeHalf),
            color
        );
    }
};

struct RigidBodyCircle : public RigidBody {
    float radius;

    RigidBodyCircle(int id, float x, float y, float radius = 1.f, float mass = 1.f,
                    float rotation = 0.f)
        : RigidBody(id, x, y, mass, rotation), radius(radius) {}

    unique_ptr<ColoredMesh> createColoredMesh(vec3 color=vec3(0, 0, 255)) {
        return make_unique<ColoredMesh>(
            Mesh::createCircle(vec2(0, 0), radius),
            color
        );
    }
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

    unique_ptr<ColoredMesh> createColoredMesh(vec3 color=vec3(0, 0, 255)) {
        vector<vec3> verticies(3);
        verticies.push_back(vec3(points[0].x, points[0].y, 0));
        verticies.push_back(vec3(points[1].x, points[1].y, 0));
        verticies.push_back(vec3(points[2].x, points[2].y, 0));
        return make_unique<ColoredMesh>(
            verticies,
            color
        );
    }
};

#endif
