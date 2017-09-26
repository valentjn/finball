#ifndef GAME_OVER_SCENE_HPP_
#define GAME_OVER_SCENE_HPP_

#include "Highscores.hpp"
#include "LevelDesign/Level.hpp"
#include "Scenes/MenuRenderer.hpp"
#include "Scenes/Scene.hpp"
#include "SDL/SDLMusic.hpp"
#include "SDL/SDLWindow.hpp"

class GameOverScene : public Scene {
private:
    const SDLWindow &window;
    SDLMusic &music;
    Highscores &highscores;
    float score;

    // TODO refactor
    Level &level;
    int frameRate;

public:
    GameOverScene(const SDLWindow &window, SDLMusic &music, Highscores &highscores, float score, Level &level, int frameRate) :
            window(window), music(music), highscores(highscores), score(score), level(level), frameRate(frameRate) {}

    std::unique_ptr<Scene> show();

private:
    void listen(MenuRenderer &menuRenderer);
};

#endif
