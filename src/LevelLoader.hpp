#ifndef LEVL_LOADER_HPP_
#define LEVL_LOADER_HPP_

#include <fstream>
#include <string>
#include <vector>

#include "Parameters.hpp"
#include "Level.hpp"
#include "Array2D.hpp"
#include "glm/vec2.hpp"

using namespace std;

class LevelLoader
{
private:
    Parameters &parameters;

public:
    LevelLoader(Parameters &parameters) : parameters(parameters)
    {}

    void loadLevel(string filePath, Level &level)
    {
        fstream file;
        file.open(filePath, fstream::in);
        if (!file.is_open())
        {
            if (parameters.verbosity_level >= 1)
            {
                cerr << "Failed to load level" << endl;
            }
            exit(EXIT_FAILURE);
        }

        string file_line;
        int width, height;
        file >> width;
        file >> height;

        Array2D<Level::CellType> *level_matrix = new Array2D<Level::CellType>(width, height);
        vector<glm::vec2> *obstacles = new vector<glm::vec2>();
        for (int y = 0; y < height; y++) {
            file >> file_line;
            for (int x = 0; x < width; x++)
            {
                Level::CellType cell = static_cast<Level::CellType>(static_cast<int>(file_line[x]) - '0');
                level_matrix->setValue(x, y, cell);
                if (cell == Level::OBSTACLE)
                {
                    obstacles->push_back(glm::vec2(x, y));
                }
            }
        }

        level.matrix = level_matrix;
        level.obstacles = obstacles;

        if (parameters.verbosity_level >= 1)
        {
            cout << "Loaded level:" << endl;
            for (int y = 0; y < height; y++)
            {
                string line = "";
                for (int x = 0; x < width; x++)
                {
                    line += to_string(level_matrix->getValue(x, y));
                }
                cout << line << endl;
            }
            if (parameters.verbosity_level >= 2)
            {
                cout << "With obstacles at:" << endl;
                for (glm::vec2 const& point: *obstacles)
                {
                    cout << "(" << point.x << "|" << point.y << ")" << endl;
                }
            }
        }
    }
};

#endif
