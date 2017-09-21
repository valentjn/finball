#ifndef LEVL_LOADER_HPP_
#define LEVL_LOADER_HPP_

#include <fstream>
#include <string>
#include <vector>

#include "Array2D.hpp"
#include "Level.hpp"
#include "Parameters.hpp"
#include "RigidBody.hpp"

using namespace std;

class LevelLoader {
private:
    Parameters &parameters;

public:
    LevelLoader(Parameters &parameters) : parameters(parameters) {}

    void loadLevel(Level &level) {
        fstream file;
        file.open(parameters.level_file_path, fstream::in);
        if (!file.is_open()) {
            if (parameters.verbosity_level >= 1) {
                cerr << "Failed to load level" << endl;
            }
            exit(EXIT_FAILURE);
        }

        string file_line;
        int width, height;
        file >> width;
        file >> height;

        Array2D<Level::CellType> *level_matrix = new Array2D<Level::CellType>(width, height);
        vector<RigidBody> *obstacles = new vector<RigidBody>();
        for (int y = 0; y < height; y++) {
            file >> file_line;
            for (int x = 0; x < width; x++) {
                Level::CellType cell =
                    static_cast<Level::CellType>(static_cast<int>(file_line[x]) - '0');
                level_matrix->value(x, y) = cell;
                if (cell == Level::OBSTACLE) {
                    obstacles->push_back(RigidBody(x, y));
                }
            }
        }

        level.width = width;
        level.height = height;
        level.matrix = level_matrix;
        level.obstacles = obstacles;

        if (parameters.verbosity_level >= 1) {
            cout << "Loaded level:" << endl;
            for (int y = 0; y < height; y++) {
                string line = "";
                for (int x = 0; x < width; x++) {
                    line += to_string(level_matrix->value(x, y));
                }
                cout << line << endl;
            }
            cout << endl;
            if (parameters.verbosity_level >= 2) {
                cout << "With obstacles at:" << endl;
                for (auto const &obstacle : *obstacles) {
                    cout << "(" << obstacle.pos.x << "|" << obstacle.pos.y << ")" << endl;
                }
            }
            cout << endl;
        }
    }
};

#endif
