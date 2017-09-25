#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#define FLIPPER_Y 2
#define FLIPPER_WIDTH 4
#define FLIPPER_GAP 2

#include <fstream>
#include <glm/glm.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "Array2D.hpp"
#include "Log.hpp"
#include "RigidBody/RigidBody.hpp"
#include "Visualization/Mesh.hpp"

using namespace std;
using namespace glm;

class Level {
private:
    unique_ptr<ColoredMesh> meshRect;
    unique_ptr<ColoredMesh> meshCircle;

public:
    enum CellType { FLUID, OBSTACLE, INFLOW, OUTFLOW };
    static const int BALL_ID = 1;

    int width, height;
    int flipperLeftId, flipperRightId;
    Array2D<CellType> matrix;
    vector<unique_ptr<RigidBody>> rigidBodies;
    unordered_map<int, Mesh*> meshes;

    Level(string levelFilePath) {
        fstream file;
        file.open(levelFilePath, fstream::in);
        if (!file.is_open()) {
            Log::error("Failed to load level");
            throw runtime_error("Failed to load level");
        }

        meshRect = make_unique<ColoredMesh>(
            Mesh::createRectangle(vec2(-0.5f, -0.5f), vec2(0.5f, 0.5f)),
            vec3(0, 0, 255)
        );

        meshCircle = make_unique<ColoredMesh>(
            Mesh::createCircle(vec2(0, 0), 2),
            vec3(255, 0, 0)
        );

        string file_line;
        file >> width >> height;

        matrix = Array2D<CellType>(width, height);
        int rigidBodyId = BALL_ID + 1;
        for (int y = height - 1; y >= 0; y--) {
            file >> file_line;
            for (int x = 0; x < width; x++) {
                if (file_line[x] == 'B') {
                    rigidBodies.push_back(make_unique<RigidBodyCircle>(BALL_ID, x, y, 2));
                    meshes[BALL_ID] = meshCircle.get();
                } else {
                    CellType cell = static_cast<CellType>(static_cast<int>(file_line[x]) - '0');
                    switch (cell) {
                        // TODO: what needs LBM??
                        case OBSTACLE:
                            rigidBodies.push_back(make_unique<RigidBodyRect>(rigidBodyId, x, y, 1, 1, 0));
                            meshes[rigidBodyId] = meshRect.get();
                            rigidBodyId++;
                            break;
                        default: matrix.value(x, y) = cell; break;
                    }
                }
            }
        }

        flipperLeftId = rigidBodyId++;
        flipperRightId = rigidBodyId++;
        createFlippers();

        debugPrint();
    }

    // delete copy constructor and copy-assignment operator
    Level(const Level &) = delete;
    Level &operator=(const Level &) = delete;

private:
    void createFlippers() {
        float leftX = width * 0.5f - FLIPPER_WIDTH - FLIPPER_GAP;
        float rightX = width * 0.5f + FLIPPER_WIDTH + FLIPPER_GAP;
        rigidBodies.push_back(make_unique<RigidBodyTriangle>(flipperLeftId, leftX, FLIPPER_Y, vec2(leftX + FLIPPER_WIDTH, FLIPPER_Y), vec2(leftX, 0), 0));
        rigidBodies.push_back(make_unique<RigidBodyTriangle>(flipperRightId, rightX, FLIPPER_Y, vec2(rightX, 0), vec2(rightX - FLIPPER_WIDTH, FLIPPER_Y), 0));

        vector<vec3> leftVerticies = {vec3(0,0,0), vec3(FLIPPER_WIDTH, 0, 0), vec3(0, -FLIPPER_Y, 0)};
        vector<vec3> rightVerticies = {vec3(0,0,0), vec3(0, -FLIPPER_Y, 0), vec3(-FLIPPER_WIDTH, 0, 0)};
        meshes[flipperLeftId] = make_unique<ColoredMesh>(leftVerticies, vec3(255, 255, 0)).get();
        meshes[flipperRightId] = make_unique<ColoredMesh>(rightVerticies, vec3(255, 255, 0)).get();
    }

    void debugPrint() {
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
            for (const unique_ptr<RigidBody> &rigidBody : rigidBodies) {
                Log::debug("(%d|%d)", (int)rigidBody->position.x, (int)rigidBody->position.y);
            }
        }
    }
};

#endif
