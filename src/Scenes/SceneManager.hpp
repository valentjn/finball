#ifndef SCENE_MANAGER_HPP_
#define SCENE_MANAGER_HPP_

#include <memory>

#include "SDL/SDLWindow.hpp"
#include "Scenes/Scene.hpp"

using namespace std;

class SceneManager {
private:
    unique_ptr<Scene> currentScene;

public:
    SceneManager(unique_ptr<Scene> scene) : currentScene(move(scene)) {}

    void run() {
        while (currentScene) {
            currentScene = currentScene->show();
        }
    }

};

#endif
