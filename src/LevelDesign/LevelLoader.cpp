#include "LevelDesign/LevelLoader.hpp"

#include "LevelDesign/Level.hpp"
#include "Log.hpp"
#include "Visualization/Mesh.hpp"

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
