#ifndef RENDERER_INPUT_HPP_
#define RENDERER_INPUT_HPP_

#include <unordered_map>
#include <vector>

#include "Array2D.hpp"
#include "GameLogic/GameLogicOutput.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"
#include "Visualization/RenderObject.hpp"
#include "Visualization/Mesh.hpp"

class RendererInput {
    std::unordered_map<int, Mesh> m_rigid_body_meshes;

public:
    std::vector<RenderObject> world_objects;
    std::vector<RenderObject> ui_objects;
    const Array2D<glm::vec2> *fluid_velocity;
    const Array2D<float> *fluid_density;

    RendererInput() : fluid_velocity(nullptr), fluid_density(nullptr) {}

    RendererInput(const GameLogicOutput &gameLogicOutput,
                  const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput,
                  const LatticeBoltzmannOutput &latticeBoltzmannOutput) {
        // handle game logic output
        world_objects.insert(world_objects.end(), gameLogicOutput.objectsToRender.begin(),
                             gameLogicOutput.objectsToRender.end());

        // handle rigid body physics output
        for (const RigidBody& rigidBody : rigidBodyPhysicsOutput.rigid_bodies) {
            RenderObject renderObject;
            renderObject.position = glm::vec3(rigidBody.position, 0);
            renderObject.scale = rigidBody.radius;
            renderObject.mesh = &m_rigid_body_meshes[rigidBody.id];
            if (*renderObject.mesh == Mesh{})
                *renderObject.mesh = createCircleMesh(1.f);
            /*switch (typeid(rigidBody)) {
            case typeid(RigidRectangle):
                const RigidRectangle& rect = static_cast<RigidRectangle>(rigidBody);
                //
                break;
            case typeid(RigidCircle):
                const RigidCircle& circle = static_cast<RigidCircle>(rigidBody);
                //
                break;
            }*/
            world_objects.push_back(renderObject);
        }

        // handle lattice boltzmann output
        fluid_velocity = &latticeBoltzmannOutput.velocity;
        fluid_density = &latticeBoltzmannOutput.density;
    }
};

#endif
