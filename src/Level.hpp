#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

#include "Visualization/Mesh.hpp"
#include "Array2D.hpp"
#include "Log.hpp"
#include "RigidBody/RigidBody.hpp"

using namespace std;

class Level {
    std::unique_ptr<Mesh> test_rigid_body_mesh;

public:
    enum CellType { FLUID, OBSTACLE, INFLOW, OUTFLOW };

    int width, height;
    Array2D<CellType> matrix;
    vector<RigidBody> rigidBodies;
    unordered_map<int, Mesh*> rigidBodyMeshes;

    Level(string levelFilePath) {
        fstream file;
        file.open(levelFilePath, fstream::in);
        if (!file.is_open()) {
            Log::error("Failed to load level");
            throw runtime_error("Failed to load level");
        }

        test_rigid_body_mesh = std::make_unique<ColoredMesh>(
            Mesh::createRectangle({-1.f, -1.f}, {1.f, 1.f}),
            std::vector<glm::vec3>{{1, 0, 0.4},{0.3, 1, 0.4},{1, 1, 0.4},{1, 0, 0.4},{1, 1, 0.4},{0, 0, 1}});

        string file_line;
        file >> width >> height;

        matrix = Array2D<CellType>(width, height);
        int rigidBodyId = 2;
        for (int y = height - 1; y >= 0; y--) {
            file >> file_line;
            for (int x = 0; x < width; x++) {
                if (file_line[x] == 'B') {
                    rigidBodies.push_back(RigidBody(1, x, y, false));
                } else {
                    CellType cell = static_cast<CellType>(static_cast<int>(file_line[x]) - '0');
                    matrix.value(x, y) = cell;
                    if (cell == OBSTACLE) {
                        rigidBodies.push_back(RigidBody(rigidBodyId++, x, y));
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
