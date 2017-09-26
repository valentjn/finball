#include <memory>

#include "GameController.hpp"
#include "LevelDesign/Level.hpp"
#include "LevelDesign/LevelLoader.hpp"
#include "Log.hpp"
#include "Scenes/MainMenuScene.hpp"
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
    {
        MainMenuScene menu(window, highscores);
        menu.show();
    }

    LevelLoader loader("data/" + parameters.level + ".txt");
    Level level;
    loader.load(level);

    // run the game
    GameController gameController(highscores, parameters.frameRate);
    gameController.startGame(window, level);

    return 0;
}
