#include <memory>

#include "LevelDesign/Level.hpp"
#include "LevelDesign/LevelLoader.hpp"
#include "Log.hpp"
#include "Scenes/MainMenuScene.hpp"
#include "Scenes/SceneManager.hpp"
#include "Scenes/SimulationScene.hpp"
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

    LevelLoader loader("data/" + parameters.level + ".txt");
    Level level;
    loader.load(level);

    // show main menu and obtain level from it

    SceneManager sceneManager(std::make_unique<MainMenuScene>(window, music, highscores, level, parameters.frameRate));
    sceneManager.run();

    return 0;
}
