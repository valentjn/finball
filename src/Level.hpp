#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Array2D.hpp"
#include "Level.hpp"
#include "Log.hpp"
#include "RigidBody.hpp"

using namespace std;

class Level {
public:
    enum CellType { FLUID, OBSTACLE, INFLOW, OUTFLOW, BALL };

    int width, height;
    Array2D<CellType> matrix;
    vector<RigidBody> rigidBodies;

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
        for (int y = height - 1; y >= 0; y--) {
            file >> file_line;
            for (int x = 0; x < width; x++) {
                CellType cell = static_cast<CellType>(static_cast<int>(file_line[x]) - '0');
                if (cell == BALL) {
                    rigidBodies.push_back(RigidBody(x, y, false));
                } else {
                    matrix.value(x, y) = cell;
                    if (cell == OBSTACLE) {
                        rigidBodies.push_back(RigidBody(x, y));
                    }
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

        Log::debug("With rigidBodies at:");
        if (Log::logLevel >= Log::DEBUG) {
            for (auto const &rigidBody : rigidBodies) {
                Log::debug("(%d|%d)", (int)rigidBody.position.x, (int)rigidBody.position.y);
            }
        }
    }

    // delete copy constructor and copy-assignment operator
    Level(const Level &) = delete;
    Level &operator=(const Level &) = delete;
};

#endif
