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
    SDLWindow window(parameters.windowWidth, parameters.windowHeight,
                     "Finball", parameters.fullscreen);
    window.setIcon("data/haicon.png");

    // show main menu and obtain level from it
    MainMenu menu(window);
    std::unique_ptr<Level> level = menu.show();

    // run the game
    GameController gameController(window, *level);
    gameController.startGame();

    return 0;
}
