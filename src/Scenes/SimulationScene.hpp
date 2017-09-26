#ifndef SIMULATION_SCENE_HPP_
#define SIMULATION_SCENE_HPP_

#include "Highscores.hpp"
#include "LevelDesign/Level.hpp"
#include "Scenes/Scene.hpp"
#include "SDL/SDLMusic.hpp"
#include "SDL/SDLWindow.hpp"
#include "UserInput/UserInput.hpp"

class SimulationScene : public Scene {
    const SDLWindow &window;
    SDLMusic &music;
    UserInput &userInput;
    Level &level;
    Highscores &highscores;
    int frameRate;

public:
    SimulationScene(const SDLWindow &window, SDLMusic &music, UserInput &userInput, Level &level, Highscores &highscores, int frameRate)
            : window(window), music(music), userInput(userInput), level(level), highscores(highscores), frameRate(frameRate) {}

    std::unique_ptr<Scene> show();
};

#endif
