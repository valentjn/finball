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

    // show main menu and obtain level from it

	Scene::Params scene_params;
	scene_params.window = &window;
	scene_params.music = &music;
	scene_params.highscores = &highscores;
	scene_params.cmd_params = &parameters;

    SceneManager(std::make_unique<MainMenuScene>(scene_params)).run();
    return 0;
}
