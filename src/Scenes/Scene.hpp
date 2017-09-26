#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <memory>

class Scene {

public:
    virtual std::unique_ptr<Scene> show() = 0;

};

#endif
