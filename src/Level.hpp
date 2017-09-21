#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <vector>

#include "Array2D.hpp"
#include "RigidBody.hpp"

class Level {
public:
    enum CellType { EMPTY, OBSTACLE, /*BC_BOUNCE_BACK,*/ BC_NO_SLIP, BC_INFLOW, BC_OUTFLOW };

    int width, height;
    Array2D<CellType> matrix;
    std::vector<RigidBody> obstacles;

    Level() : width(-1), height(-1) {}

    // delete copy constructor and copy-assignent operator
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;
};

#endif
