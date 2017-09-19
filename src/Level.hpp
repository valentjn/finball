#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include "Array2D.hpp"

class Level {
public:
    enum CellType
    {
        EMPTY, OBSTACLE, BC_BOUNCE_BACK, BC_NO_SLIP, BC_INFLOW, BC_OUTFLOW
    };

    Array2D<CellType> *matrix;

    Level() : matrix(nullptr)
    {}

    ~Level()
    {
        if (matrix != nullptr)
        {
            delete matrix;
        }
    }
};

#endif
