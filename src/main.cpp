#include "Log.hpp"
#include "MainMenu.hpp"
#include "Parameters.hpp"

int main(int argc, char *argv[]) {
    Parameters parameters(argc, argv);
    Log::setLogLevel(parameters.verbosityLevel);

    MainMenu menu;
    menu.show();

    /*
    GameController gameController;
    gameController.startGame(Level("data/testLevel.txt"));
    */

    return 0;
}
