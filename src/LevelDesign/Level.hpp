#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <memory>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "Log.hpp"
#include "Array2D.hpp"
#include "RigidBody/RigidBody.hpp"
#include "Visualization/Mesh.hpp"

using namespace std;
using namespace glm;

struct Level {
private:
    vector<unique_ptr<Mesh>> unique_meshes;

public:
    enum CellType { FLUID, OBSTACLE, INFLOW, OUTFLOW };
    static const int BALL_ID;

    int width, height;
    int flipperLeftId, flipperRightId;
    Array2D<CellType> matrix;
    vector<unique_ptr<RigidBody>> rigidBodies;
    unordered_map<int, Mesh*> meshes;

    // delete copy constructor and copy-assignment operator
    Level(const Level &) = delete;
    Level &operator=(const Level &) = delete;

    Level() {}

    void setBallPosition(float xpos, float ypos) {
        for (const auto &rb : rigidBodies) {
            if (rb->id == Level::BALL_ID) {
                rb->position = {xpos, ypos};
            }
        }
    }

    void setUniqueMesh(int id, unique_ptr<Mesh> mesh) {
        Log::debug("starting setUn");
        unique_meshes.push_back(move(mesh));
        Log::debug("pushed to unique m");
        meshes[id] = mesh.get();
        Log::debug("done");
    }

    Mesh *addUniqueMesh(unique_ptr<Mesh> mesh) {
        unique_meshes.push_back(move(mesh));
        return mesh.get();
    }
};

#endif
