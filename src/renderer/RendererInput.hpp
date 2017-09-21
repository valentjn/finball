#ifndef RENDERER_INPUT_HPP_
#define RENDERER_INPUT_HPP_

#include <vector>

#include "GameLogicOutput.hpp"
#include "LatticeBoltzmannOutput.hpp"
#include "RenderObject.hpp"
#include "RigidBodyPhysicsOutput.hpp"
#include <Array2D.hpp>

class RendererInput {
public:
    std::vector<RenderObject> world_objects;
    std::vector<RenderObject> ui_objects;
    const Array2D<glm::vec2>* fluid_velocity;
    const Array2D<float>* fluid_density;

    RendererInput() : fluid_velocity(nullptr), fluid_density(nullptr) {}

    RendererInput(const GameLogicOutput &gameLogicOutput,
                  const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput,
                  const LatticeBoltzmannOutput &latticeBoltzmannOutput)
    {
        // handle game logic output
        world_objects.insert(world_objects.end(), gameLogicOutput.objectsToRender.begin(),
                             gameLogicOutput.objectsToRender.end());

        // handle rigid body physics output
        for (RigidBody rigidBody : rigidBodyPhysicsOutput.rigid_bodies) {
            RenderObject renderObject;
            renderObject.position = glm::vec3(rigidBody.position, 0);
            renderObject.scale = rigidBody.radius;
            world_objects.push_back(renderObject);
        }

        // handle lattice boltzmann output
        fluid_velocity = &latticeBoltzmannOutput.velocity;
        fluid_density = &latticeBoltzmannOutput.density;
    }
};

#endif
