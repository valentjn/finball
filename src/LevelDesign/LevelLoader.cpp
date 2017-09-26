#include "LevelDesign/LevelLoader.hpp"

#include "LevelDesign/Level.hpp"
#include "Log.hpp"
#include "Visualization/Mesh.hpp"

const int LevelLoader::FLIPPER_Y = 2;
const int LevelLoader::FLIPPER_WIDTH = 8;
const int LevelLoader::FLIPPER_GAP = 2;

void LevelLoader::createFlippers(Level &level) {
    float leftX = level.width * 0.5f - FLIPPER_WIDTH - FLIPPER_GAP;
    float rightX = level.width * 0.5f + FLIPPER_WIDTH + FLIPPER_GAP;

    auto rigidBodyLeft = make_unique<RigidBodyTriangle>(level.flipperLeftId, leftX, FLIPPER_Y,
                                                        vec2(FLIPPER_WIDTH, -FLIPPER_Y),
                                                        vec2(0, -FLIPPER_Y),
                                                        0);
    auto rigidBodyRight = make_unique<RigidBodyTriangle>(level.flipperRightId, rightX, FLIPPER_Y,
                                                         vec2(0, -FLIPPER_Y),
                                                         vec2(-FLIPPER_WIDTH, -FLIPPER_Y),
                                                         0);

    level.setUniqueMesh(level.flipperLeftId, rigidBodyLeft->createColoredMesh(vec3(255, 255, 0)));
    level.setUniqueMesh(level.flipperRightId, rigidBodyRight->createColoredMesh(vec3(255, 255, 0)));

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
