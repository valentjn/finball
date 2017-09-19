#ifndef LEVL_LOADER_HPP_
#define LEVL_LOADER_HPP_

#include <fstream>
#include <string>

#include "Parameters.hpp"
#include "Array2D.hpp"

using namespace std;

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

    Array2D<CellType> loadLevel(string filePath)
    {
        fstream file;
        file.open(filePath, fstream::in);

        string file_line;
        int width, height;
        file >> width;
        file >> height;

        Array2D<CellType> level(width, height);
        for (int y = 0; y < height; y++) {
            file >> file_line;
            for (int x = 0; x < width; x++)
            {
                int cell = static_cast<int>(file_line[x]) - '0';
                level.setValue(x, y, static_cast<CellType>(cell));
            }
        }

        // for (int x = 0; x < 10; x++)
        // {
        //     for (int y = 0; y < 10; y++)
        //     {
        //         if (x == 0 || y == 0)
        //             level.setValue(x, y, BC_INFLOW);
        //         else if (x == 9 || y == 9)
        //             level.setValue(x, y, BC_OUTFLOW);
        //         else
        //             level.setValue(x, y, EMPTY);
        //     }
        // }

        for (int y = 0; y < height; y++)
        {
            string line = "";
            for (int x = 0; x < width; x++)
            {
                line += to_string(level.getValue(x, y));
            }
            cout << line << endl;
        }

        return level;
    }
};

#endif
