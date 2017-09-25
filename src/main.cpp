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

    Highscores highscores("haiscores.txt");

    // show main menu and obtain level from it
    std::unique_ptr<Level> level;
    {
        MainMenu menu(window, highscores);
        level = menu.show();
    }

    // run the game
    GameController gameController(highscores);
    gameController.startGame(window, *level);

    return 0;
}
