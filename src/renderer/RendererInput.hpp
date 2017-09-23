#ifndef RENDERER_INPUT_HPP_
#define RENDERER_INPUT_HPP_

#include <vector>

#include <GameLogicOutput.hpp>
#include <LatticeBoltzmannOutput.hpp>
#include <renderer/RenderObject.hpp>
#include <RigidBodyPhysicsOutput.hpp>
#include <Array2D.hpp>
#include <renderer/Mesh.hpp>

class RendererInput {
    static Mesh m_rectangle;
    static Mesh m_circle;
    static bool m_meshes_initialized;

public:
    std::vector<RenderObject> world_objects;
    std::vector<RenderObject> ui_objects;
    const Array2D<glm::vec2> *fluid_velocity;
    const Array2D<float> *fluid_density;

    RendererInput() : fluid_velocity(nullptr), fluid_density(nullptr) {}

    RendererInput(const GameLogicOutput &gameLogicOutput,
                  const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput,
                  const LatticeBoltzmannOutput &latticeBoltzmannOutput) {
        if (!m_meshes_initialized) {
            m_rectangle = createRectangleMesh(2.f, 2.f);
            m_circle = createCircleMesh(1.f);
        }

        // handle game logic output
        world_objects.insert(world_objects.end(), gameLogicOutput.objectsToRender.begin(),
                             gameLogicOutput.objectsToRender.end());

        // handle rigid body physics output
        for (const RigidBody& rigidBody : rigidBodyPhysicsOutput.rigid_bodies) {
            RenderObject renderObject;
            renderObject.position = glm::vec3(rigidBody.position, 0);
            renderObject.scale = rigidBody.radius;
            renderObject.mesh = &m_circle;
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
