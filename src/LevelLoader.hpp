#ifndef LEVL_LOADER_HPP_
#define LEVL_LOADER_HPP_

#include <iostream>
#include <string>

#include "Parameters.hpp"
#include "Array2D.hpp"

class LevelLoader
{
public:
    enum CellType
    {
        EMPTY, OBSTACLE, BC_BOUNCE_BACK, BC_NO_SLIP, BC_INFLOW, BC_OUTFLOW
    };

private:
    Parameters &parameters;

public:
    LevelLoader(Parameters &parameters) : parameters(parameters)
    {}

    // TODO: level name/path from parameters??
    Array2D<CellType> loadLevel(std::string filePath)
    {
        // TODO: Load level from file

        Array2D<CellType> level(10, 10);
        for (int x = 0; x < 10; x++)
        {
            for (int y = 0; y < 10; y++)
            {
                if (x == 0 || y == 0)
                    level.setValue(x, y, BC_INFLOW);
                else if (x == 9 || y == 9)
                    level.setValue(x, y, BC_OUTFLOW);
                else
                    level.setValue(x, y, EMPTY);
            }
        }

        for (int y = 0; y < 10; y++)
        {
            std::string line = "";
            for (int x = 0; x < 10; x++)
            {
                line += std::to_string(level.getValue(x, y));
            }
            std::cout << line << std::endl;
        }

        return level;
    }
};

#endif
