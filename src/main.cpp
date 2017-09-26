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

    // load highscores
    Highscores highscores("haiscores.txt");

    Scene::Context sceneContext {
        &window,
        &music,
        &highscores,
        &parameters
    };

    SceneManager sceneManager(std::make_unique<MainMenuScene>(sceneContext));
    sceneManager.run();

    return 0;
}
