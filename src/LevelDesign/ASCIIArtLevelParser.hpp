#ifndef ASCII_ART_LEVEL_PARSER_HPP_
#define ASCII_ART_LEVEL_PARSER_HPP_

#include <fstream>
#include <glm/glm.hpp>
#include <stdexcept>
#include <string>

#include "Array2D.hpp"
#include "LevelDesign/Level.hpp"
#include "Log.hpp"
#include "RigidBody/RigidBody.hpp"
#include "Visualization/Mesh.hpp"

using namespace std;

class ASCIIArtLevelParser {

private:
    string filePath;

public:
    ASCIIArtLevelParser(string filePath) : filePath(filePath) {}

    void parse(Level &level) {
        fstream file;
        file.open(filePath, fstream::in);
        if (!file.is_open()) {
            Log::error("Failed to load level %s", filePath);
            throw runtime_error("Failed to load level");
        }

        unique_ptr<ColoredMesh> meshRect = make_unique<ColoredMesh>(
            Mesh::createRectangle(vec2(-0.5f, -0.5f), vec2(0.5f, 0.5f)),
            vec3(0, 0, 255)
        );

        unique_ptr<ColoredMesh> meshCircle = make_unique<ColoredMesh>(
            Mesh::createCircle(vec2(0, 0), 2),
            vec3(255, 0, 0)
        );

        string file_line;
        file >> level.width >> level.height;

        level.matrix = Array2D<Level::CellType>(level.width, level.height);
        int rigidBodyId = Level::BALL_ID + 1;
        for (int y = level.height - 1; y >= 0; y--) {
            file >> file_line;
            for (int x = 0; x < level.width; x++) {
                if (file_line[x] == 'B') {
                    level.rigidBodies.push_back(make_unique<RigidBodyCircle>(Level::BALL_ID, x, y, 1));
                    level.meshes[Level::BALL_ID] = meshCircle.get();
                } else {
                    Level::CellType cell = static_cast<Level::CellType>(static_cast<int>(file_line[x]) - '0');
                    if (cell == Level::CellType::OBSTACLE) {
                        level.rigidBodies.push_back(make_unique<RigidBodyRect>(rigidBodyId, x, y, 1, 1, 0));
                        level.meshes[rigidBodyId] = meshRect.get();
                        rigidBodyId++;
                    }
                    level.matrix.value(x, y) = cell;
                }
            }
        }

        level.flipperLeftId = rigidBodyId++;
        level.flipperRightId = rigidBodyId++;
    }

};

#endif
