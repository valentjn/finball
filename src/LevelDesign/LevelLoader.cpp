#include "LevelDesign/LevelLoader.hpp"

#include "LevelDesign/Level.hpp"
#include "Log.hpp"
#include "Visualization/Mesh.hpp"

void LevelLoader::createFlippers(Level &level) {
    float leftX = level.width * 0.5f - Level::FLIPPER_WIDTH - Level::FLIPPER_GAP;
    float rightX = level.width * 0.5f + Level::FLIPPER_WIDTH + Level::FLIPPER_GAP;

    // positive masses as they aren't static objects
    auto rigidBodyLeft = make_unique<RigidBodyTriangle>(level.flipperLeftId, leftX, Level::FLIPPER_Y,
                                                        vec2(Level::FLIPPER_WIDTH, -Level::FLIPPER_HEIGHT),
                                                        vec2(0.f, -Level::FLIPPER_HEIGHT),
                                                        1);
    auto rigidBodyRight = make_unique<RigidBodyTriangle>(level.flipperRightId, rightX, Level::FLIPPER_Y,
                                                         vec2(0.f, -Level::FLIPPER_HEIGHT),
                                                         vec2(-Level::FLIPPER_WIDTH, -Level::FLIPPER_HEIGHT),
                                                         1);
#ifdef OPENCV_LIBS
    unique_ptr<Mesh> meshLeft, meshRight;
    if ((meshLeft = Mesh::createImageMesh("data/fin_left.png", level.leftFinTexture, Level::FLIPPER_WIDTH)) == nullptr) {
        meshLeft = rigidBodyLeft->createColoredMesh(Level::FLIPPER_COLOR);
    }
    if ((meshRight = Mesh::createImageMesh("data/fin_right.png", level.rightFinTexture, Level::FLIPPER_WIDTH)) == nullptr) {
        meshRight = rigidBodyRight->createColoredMesh(Level::FLIPPER_COLOR);
    }
    level.setUniqueMesh(level.flipperLeftId, move(meshLeft));
    level.setUniqueMesh(level.flipperRightId, move(meshRight));
#else
    level.setUniqueMesh(level.flipperLeftId, rigidBodyLeft->createColoredMesh(Level::FLIPPER_COLOR));
    level.setUniqueMesh(level.flipperRightId, rigidBodyRight->createColoredMesh(Level::FLIPPER_COLOR));
#endif

    level.rigidBodies.push_back(std::move(rigidBodyLeft));
    level.rigidBodies.push_back(std::move(rigidBodyRight));
}

void LevelLoader::debugPrint(Level &level) {
    Log::info("Loaded level:");
    if (Log::logLevel >= Log::INFO) {
        for (int y = level.height - 1; y >= 0; y--) {
            string line = "";
            for (int x = 0; x < level.width; x++) {
                line += to_string(level.matrix.value(x, y));
            }
            Log::info(line);
        }
    }

    Log::debug("With rigidBodies at:");
    if (Log::logLevel >= Log::DEBUG) {
        for (const unique_ptr<RigidBody> &rigidBody : level.rigidBodies) {
            Log::debug("(%d|%d)", (int)rigidBody->position.x, (int)rigidBody->position.y);
        }
    }
}
