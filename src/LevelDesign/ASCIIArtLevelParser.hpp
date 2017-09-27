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
#include "Visualization/Texture.hpp"

#ifdef OPENCV_LIBS
#include <opencv2/opencv.hpp>
#endif

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
            Log::error("Failed to load level %s", filePath.c_str());
            return false;
        }

        Mesh *meshRect = level.addUniqueMesh(make_unique<ColoredMesh>(
            Mesh::createRectangle(vec2(-0.5f, -0.5f), vec2(0.5f, 0.5f)),
            Level::OBSTACLE_COLOR
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
#ifdef OPENCV_LIBS
                    cv::Mat img = cv::imread(Level::BALL_IMAGE_PATH, cv::IMREAD_UNCHANGED);
                    unique_ptr<Texture4F> texture = make_unique<Texture4F>(glm::ivec2{img.size().width, img.size().height});
                    texture->setData(img);
                    level.addUniqueTexture(make_unique<Texture>(static_cast<Texture>(*texture))); // TODO: WTF, there must be a better way...
                    level.setUniqueMesh(Level::BALL_ID, make_unique<TexturedMesh>(
                        Mesh::createCircle(vec2(0, 0), rigidBody->radius),
                        texture.get()
                    ));
#else
                    level.setUniqueMesh(Level::BALL_ID, rigidBody->createColoredMesh(Level::BALL_COLOR));
#endif
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
