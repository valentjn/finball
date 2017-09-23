#include <memory>

#include "GameController.hpp"
#include "Log.hpp"
#include "MainMenu.hpp"
#include "Parameters.hpp"
#include "SDL/SDLWindow.hpp"

int main(int argc, char *argv[]) {

    // read parameters
    Parameters parameters(argc, argv);

    // create log
    Log::setLogLevel(parameters.verbosityLevel);

    // create SDL Controller
    SDLWindow window(800, 600, "Finball", parameters.fullscreen);

    // show main menu and obtain level from it
    MainMenu menu(window);
    std::unique_ptr<Level> level = menu.show();

    // run the game
    GameController gameController;
    gameController.startGame(window, *level);

    return 0;
}
