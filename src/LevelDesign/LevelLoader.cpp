#include "LevelDesign/LevelLoader.hpp"

#include "LevelDesign/Level.hpp"
#include "Log.hpp"
#include "Visualization/Mesh.hpp"

const float LevelLoader::FLIPPER_Y = 2.5f;
const float LevelLoader::FLIPPER_WIDTH = 8.f;
const float LevelLoader::FLIPPER_HEIGHT = 3.f;
const float LevelLoader::FLIPPER_GAP = 2.5f;

void LevelLoader::createFlippers(Level &level) {
    float leftX = level.width * 0.5f - FLIPPER_WIDTH - FLIPPER_GAP;
    float rightX = level.width * 0.5f + FLIPPER_WIDTH + FLIPPER_GAP;

    auto rigidBodyLeft = make_unique<RigidBodyTriangle>(level.flipperLeftId, leftX, FLIPPER_Y,
                                                        vec2(FLIPPER_WIDTH, -FLIPPER_HEIGHT),
                                                        vec2(0.f, -FLIPPER_HEIGHT),
                                                        0);
    auto rigidBodyRight = make_unique<RigidBodyTriangle>(level.flipperRightId, rightX, FLIPPER_Y,
                                                         vec2(0.f, -FLIPPER_HEIGHT),
                                                         vec2(-FLIPPER_WIDTH, -FLIPPER_HEIGHT),
                                                         0);

    level.setUniqueMesh(level.flipperLeftId, rigidBodyLeft->createColoredMesh(Level::FLIPPER_COLOR));
    level.setUniqueMesh(level.flipperRightId, rigidBodyRight->createColoredMesh(Level::FLIPPER_COLOR));

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
