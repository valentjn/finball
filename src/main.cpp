#include <memory>

#include "GameController.hpp"
#include "Log.hpp"
#include "MainMenu.hpp"
#include "Parameters.hpp"
#include "SDLController.hpp"

int main(int argc, char *argv[]) {

    // read parameters
    Parameters parameters(argc, argv);

    // create log
    Log::setLogLevel(parameters.verbosityLevel);

    // create SDL Controller
    SDLController::getInstance().startSDL();

    // show main menu and obtain level from it
    MainMenu menu;
    std::unique_ptr<Level> level = menu.show();

    // run the game
    GameController gameController;
    gameController.startGame(*level);

    return 0;
}
