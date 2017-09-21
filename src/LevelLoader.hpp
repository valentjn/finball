#ifndef LEVL_LOADER_HPP_
#define LEVL_LOADER_HPP_

#include <fstream>
#include <string>
#include <vector>

#include "Array2D.hpp"
#include "Level.hpp"
#include "Log.hpp"
#include "RigidBody.hpp"

using namespace std;

class LevelLoader {
public:
    void loadLevel(Level &level) {
        fstream file;
        file.open("data/testLevel.txt", fstream::in);
        if (!file.is_open()) {
            Log::error("Failed to load level");
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

        Log::info("Loaded level:");
        if (Log::logLevel >= Log::INFO) {
            for (int y = 0; y < height; y++) {
                string line = "";
                for (int x = 0; x < width; x++) {
                    line += to_string(level_matrix->value(x, y));
                }
                Log::info(line);
            }
        }

        Log::debug("With obstacles at:");
        if (Log::logLevel >= Log::LogLevel::DEBUG) {
            for (auto const &obstacle : *obstacles) {
                Log::debug("(%d|%d)", (int)obstacle.pos.x, (int)obstacle.pos.y);
            }
        }
    }
};

#endif
