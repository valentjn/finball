#include "Log.hpp"
#include "MainMenu.hpp"
#include "Parameters.hpp"
#include "SDLController.hpp"

int main(int argc, char *argv[]) {
    Parameters parameters(argc, argv);
    Log::setLogLevel(parameters.verbosityLevel);
    SDLController::getInstance().startSDL();

    MainMenu menu;
    menu.show();

    /*
    Level level("data/testLevel.txt");
    GameController gameController;
    gameController.startGame(level);
    */

    return 0;
}
