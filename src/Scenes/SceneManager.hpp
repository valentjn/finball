#ifndef SCENE_MANAGER_HPP_
#define SCENE_MANAGER_HPP_

#include <memory>

#include "SDL/SDLWindow.hpp"
#include "Scenes/Scene.hpp"

class SceneManager {
private:
    std::unique_ptr<Scene> currentScene;

public:
    SceneManager(std::unique_ptr<Scene> scene) : currentScene(std::move(scene)) {}

    void run() {
        while (currentScene)
            currentScene = currentScene->show();
    }

};

#endif
