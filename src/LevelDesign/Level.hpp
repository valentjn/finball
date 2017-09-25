#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <memory>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "Array2D.hpp"
#include "RigidBody/RigidBody.hpp"
#include "Visualization/Mesh.hpp"

using namespace std;
using namespace glm;

struct Level {
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

    void setBallPosition(float xpos, float ypos){
        for (const auto &rb : rigidBodies) {
            if (rb->id == Level::BALL_ID) {
                rb->position = {xpos, ypos};
            }
        }
    }

};

#endif
