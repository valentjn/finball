#ifndef ASCII_ART_LEVEL_PARSER_HPP_
#define ASCII_ART_LEVEL_PARSER_HPP_

#include <fstream>
#include <glm/glm.hpp>
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

    bool parse(Level &level) {
        fstream file;
        file.open(filePath, fstream::in);
        if (!file.is_open()) {
            Log::error("Failed to load level %s", filePath);
            return false;
        }

        Mesh *meshRect = level.addUniqueMesh(make_unique<ColoredMesh>(
            Mesh::createRectangle(vec2(-0.5f, -0.5f), vec2(0.5f, 0.5f)),
            vec3(0, 0, 255)
        ));

        string file_line;
        file >> level.width >> level.height;

        level.matrix = Array2D<Level::CellType>(level.width, level.height);
        int rigidBodyId = Level::BALL_ID + 1;
        for (int y = level.height - 1; y >= 0; y--) {
            file >> file_line;
            for (int x = 0; x < level.width; x++) {
                if (file_line[x] == 'B') {
                    auto rigidBody = make_unique<RigidBodyCircle>(Level::BALL_ID, x, y);
                    level.setUniqueMesh(Level::BALL_ID, rigidBody->createColoredMesh(vec3(255, 0, 0)));
                    level.rigidBodies.push_back(move(rigidBody));
                } else {
                    Level::CellType cell = static_cast<Level::CellType>(static_cast<int>(file_line[x]) - '0');
                    if (cell == Level::CellType::OBSTACLE) {
                        level.rigidBodies.push_back(make_unique<RigidBodyRect>(rigidBodyId, x, y, 1, 1, 0));
                        level.meshes[rigidBodyId] = meshRect;
                        rigidBodyId++;
                    }
                    level.matrix.value(x, y) = cell;
                }
            }
        }

        level.flipperLeftId = rigidBodyId++;
        level.flipperRightId = rigidBodyId++;
        return true;
    }
};

#endif
