#include "MainMenu.hpp"
#include "Parameters.hpp"

int main(int argc, char *argv[]) {
    Parameters parameters;
    parameters.setup(argc, argv);

    MainMenu menu(parameters);
    menu.show();

    /*
    Level level;
    LevelLoader levelLoader(parameters);
    levelLoader.loadLevel(level);

    GameController gameController(parameters);
    gameController.startGame(level);
    */

    return 0;
}
