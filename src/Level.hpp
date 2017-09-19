#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <vector>

#include "Array2D.hpp"
#include "Vector2.hpp"

class Level {
public:
    enum CellType
    {
        EMPTY, OBSTACLE, BC_BOUNCE_BACK, BC_NO_SLIP, BC_INFLOW, BC_OUTFLOW
    };

    Array2D<CellType> *matrix;
    std::vector<Vector2> *obstacles;

    Level() :
        matrix(nullptr),
        obstacles(nullptr)
    {}

    ~Level()
    {
        if (matrix != nullptr)
        {
            delete matrix;
        }
        if (obstacles != nullptr)
        {
            delete obstacles;
        }
    }
};

#endif
