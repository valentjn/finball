#include <memory>

#include "GameController.hpp"
#include "LevelDesign/Level.hpp"
#include "LevelDesign/LevelLoader.hpp"
#include "Log.hpp"
#include "Scenes/MainMenuScene.hpp"
#include "Parameters.hpp"
#include "SDL/SDLWindow.hpp"
#include "SDL/SDLMusic.hpp"


int main(int argc, char *argv[]) {

    // read parameters
    Parameters parameters(argc, argv);

    // create log
    Log::setLogLevel(parameters.verbosityLevel);

    // create SDL Controller
    SDLWindow window(parameters.windowWidth, parameters.windowHeight,
                     "Finball", parameters.fullscreen);
    window.setIcon("data/haicon.png");

    SDLMusic music;

    Highscores highscores("haiscores.txt");

    music.load("data/MainTheme.mp3");

    // show main menu and obtain level from it
    {
        MainMenuScene menu(window, highscores);
        menu.show();
    }

    music.load("data/GameTheme.mp3");

    LevelLoader loader("data/" + parameters.level + ".txt");
    Level level;
    loader.load(level);

    // run the game
    GameController gameController(highscores, parameters.frameRate);
    gameController.startGame(window, level);

    return 0;
}
