#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <vector>

#include "Array2D.hpp"
#include "RigidBody.hpp"

class Level {
public:
    enum CellType { FLUID, OBSTACLE, INFLOW, OUTFLOW };

    int width, height;
    Array2D<CellType> *matrix;
    std::vector<RigidBody> *obstacles;

    Level() : width(-1), height(-1), matrix(nullptr), obstacles(nullptr) {}

    ~Level() {
        if (matrix != nullptr) {
            delete matrix;
        }
        if (obstacles != nullptr) {
            delete obstacles;
        }
    }
};

#endif
