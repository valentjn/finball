#include "GameController.hpp"
#include "Level.hpp"
#include "LevelLoader.hpp"

Parameters parameters;

int main(int argc, char *argv[]) {
    parameters.setup(argc, argv);

    Level level;
    LevelLoader levelLoader(parameters);
    levelLoader.loadLevel(level);

    GameController gameController(parameters);
    gameController.startGame(level);

    return 0;
}
