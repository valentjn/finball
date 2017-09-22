#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <vector>
#include <stdexcept>
#include <fstream>
#include <string>

#include "Array2D.hpp"
#include "Level.hpp"
#include "Log.hpp"
#include "RigidBody.hpp"

using namespace std;

class Level {
public:
    enum CellType { FLUID, OBSTACLE, INFLOW, OUTFLOW };

    int width, height;
    Array2D<CellType> matrix;
    vector<RigidBody> obstacles;

    Level(string levelFilePath) {
        fstream file;
        file.open(levelFilePath, fstream::in);
        if (!file.is_open()) {
            Log::error("Failed to load level");
            throw runtime_error("Failed to load level");
        }

        string file_line;
        file >> width >> height;

        matrix = Array2D<CellType>(width, height);
        for (int y = 0; y < height; y++) {
            file >> file_line;
            for (int x = 0; x < width; x++) {
                CellType cell = static_cast<CellType>(static_cast<int>(file_line[x]) - '0');
                matrix.value(x, height - y - 1) = cell;
                if (cell == OBSTACLE) {
                    obstacles.push_back(RigidBody(x, height - y - 1));
                }
            }
        }

        Log::info("Loaded level:");
        if (Log::logLevel >= Log::INFO) {
            for (int y = height - 1; y >= 0; y--) {
                string line = "";
                for (int x = 0; x < width; x++) {
                    line += to_string(matrix.value(x, y));
                }
                Log::info(line);
            }
        }

        Log::debug("With obstacles at:");
        if (Log::logLevel >= Log::DEBUG) {
            for (auto const &obstacle : obstacles) {
                Log::debug("(%d|%d)", (int)obstacle.position.x, (int)obstacle.position.y);
            }
        }
    }

    // delete copy constructor and copy-assignment operator
    Level(const Level&) = delete;
    Level& operator=(const Level&) = delete;
};

#endif
