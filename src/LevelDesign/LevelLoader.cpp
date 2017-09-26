#include "LevelDesign/LevelLoader.hpp"

#include "LevelDesign/Level.hpp"
#include "Log.hpp"
#include "Visualization/Mesh.hpp"

const int LevelLoader::FLIPPER_Y = 2;
const int LevelLoader::FLIPPER_WIDTH = 4;
const int LevelLoader::FLIPPER_GAP = 2;

void LevelLoader::createFlippers(Level &level) {
    float leftX = level.width * 0.5f - FLIPPER_WIDTH - FLIPPER_GAP;
    float rightX = level.width * 0.5f + FLIPPER_WIDTH + FLIPPER_GAP;

    level.rigidBodies.push_back(make_unique<RigidBodyTriangle>(level.flipperLeftId, leftX, FLIPPER_Y, vec2(leftX + FLIPPER_WIDTH, FLIPPER_Y), vec2(leftX, 0), 0));
    level.rigidBodies.push_back(make_unique<RigidBodyTriangle>(level.flipperRightId, rightX, FLIPPER_Y, vec2(rightX, 0), vec2(rightX - FLIPPER_WIDTH, FLIPPER_Y), 0));

    vector<vec3> leftVerticies = {vec3(0,0,0), vec3(FLIPPER_WIDTH, 0, 0), vec3(0, -FLIPPER_Y, 0)};
    vector<vec3> rightVerticies = {vec3(0,0,0), vec3(0, -FLIPPER_Y, 0), vec3(-FLIPPER_WIDTH, 0, 0)};
    level.meshes[level.flipperLeftId] = make_unique<ColoredMesh>(leftVerticies, vec3(255, 255, 0)).get();
    level.meshes[level.flipperRightId] = make_unique<ColoredMesh>(rightVerticies, vec3(255, 255, 0)).get();
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
