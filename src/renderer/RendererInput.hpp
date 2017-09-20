#ifndef RENDERER_INPUT_HPP_
#define RENDERER_INPUT_HPP_

#include <vector>

#include "GameLogicOutput.hpp"
#include "LatticeBoltzmannOutput.hpp"
#include "RenderObject.hpp"
#include "RigidBodyPhysicsOutput.hpp"

class RendererInput {
  public:
    std::vector<RenderObject> world_objects;
    std::vector<RenderObject> ui_objects;

    RendererInput() {}

    RendererInput(const GameLogicOutput &gameLogicOutput,
                  const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput,
                  const LatticeBoltzmannOutput &latticeBoltzmannOutput) {
        world_objects.insert(world_objects.end(),
                             gameLogicOutput.objectsToRender.begin(),
                             gameLogicOutput.objectsToRender.end());
    }
};

#endif
