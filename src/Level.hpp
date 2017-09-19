#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <vector>

#include "Array2D.hpp"
#include "glm/vec2.hpp"

class Level {
public:
    enum CellType
    {
        EMPTY, OBSTACLE, BC_BOUNCE_BACK, BC_NO_SLIP, BC_INFLOW, BC_OUTFLOW
    };

    int width, height;
    Array2D<CellType> *matrix;
    std::vector<glm::vec2> *obstacles;

    Level() :
        width(-1),
        height(-1),
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
