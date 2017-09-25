#ifndef RENDERER_INPUT_HPP_
#define RENDERER_INPUT_HPP_

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

#include "Array2D.hpp"
#include "GameLogic/GameLogicOutput.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"
#include "Visualization/RenderObject.hpp"
#include "Visualization/Mesh.hpp"
#include "Texture.hpp"

class RendererInput {
    std::unordered_map<int, std::unique_ptr<Mesh>> m_rigid_body_meshes;

public:
    std::vector<RenderObject> world_objects;
    std::vector<RenderObject> ui_objects;
    Array2D<glm::vec3> fluid_input;
    TexturedMesh* fluid_mesh;

    RendererInput() : fluid_mesh(nullptr) {}

    RendererInput(const GameLogicOutput &gameLogicOutput,
                  const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput,
                  const LatticeBoltzmannOutput &latticeBoltzmannOutput)
    {   // handle game logic output
        world_objects.insert(world_objects.end(), gameLogicOutput.objectsToRender.begin(),
                             gameLogicOutput.objectsToRender.end());

        static std::unique_ptr<Mesh> dummy_mesh;
        if (!dummy_mesh) {
            auto square = Mesh::createRectangle({-1.f, -1.f}, {1.f, 1.f});
            dummy_mesh = std::make_unique<ColoredMesh>(
                square,
                std::vector<glm::vec3>{{1, 0, 0.4},{0.3, 1, 0.4},{1, 1, 0.4},{1, 0, 0.4},{1, 1, 0.4},{0, 0, 1}});
        }

        for (auto& gameLogicObject : world_objects) {
        	if (!gameLogicObject.mesh) {
				Log::warn("A game_logic object does not have a render mesh, rendering as rectangle");
                gameLogicObject.mesh = dummy_mesh.get();
        	}
        }

        Log::info("Rendering %d world object of gameLogic.", world_objects.size());

        fluid_mesh = gameLogicOutput.fluid_mesh;

        // handle rigid body physics output
		for (const RigidBody* rigidBody : rigidBodyPhysicsOutput.rigid_bodies) {
            const Mesh* mesh = m_rigid_body_meshes[rigidBody->id].get();
            if (!mesh) { // check if the mesh wasn't already in the map
				//Log::info(
                //    "WARNING: rigid body with id %d does not have a render mesh, rendering as circle",
                //    rigidBody->id);
                mesh = dummy_mesh.get();
            }
            RenderObject renderObject;
            renderObject.position = glm::vec3(rigidBody->position, 0);
            renderObject.scale = { rigidBody->radius, rigidBody->radius };
            renderObject.rotation = rigidBody->angle;
			assert(m_rigid_body_meshes.count(rigidBody->id)!=0);
			renderObject.mesh = mesh;

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


        // visualise the flag field from rigidbody
        static std::unique_ptr<Texture3F> ff_texture;
        static std::unique_ptr<TexturedMesh> flag_field_mesh;
        const int ff_width = rigidBodyPhysicsOutput.grid_objects.width();
        const int ff_height = rigidBodyPhysicsOutput.grid_objects.height();
        if (!flag_field_mesh) {
            ff_texture = std::make_unique<Texture3F>(glm::ivec2(ff_width, ff_height), false);
            const std::vector<glm::vec3> flag_field_quad = Mesh::createRectangle(glm::vec2{-1, -1}, glm::vec2{1, 1});
            flag_field_mesh = std::make_unique<TexturedMesh>(flag_field_quad, ff_texture.get());
        }
        // fill the texture with data from the flag field
        Array2D<glm::vec3> ff_data = Array2D<glm::vec3>(ff_width, ff_height);
        for (int x = 0; x < ff_width; x++) {
            for (int y = 0; y < ff_height; y++) {
                switch (rigidBodyPhysicsOutput.grid_objects.value(x, y)) {
                    case Level::CellType::FLUID:
                        ff_data.value(x, y) = glm::vec3{0.0f, 0.0f, 1.0f}; // blue
                        break;
                    case Level::CellType::OBSTACLE:
                        ff_data.value(x, y) = glm::vec3{0.56f, 0.37f, 0.2f}; // brown
                        break;
                    case Level::CellType::INFLOW:
                        ff_data.value(x, y) = glm::vec3{ 0.0f, 1.0f, 0.0f }; // green
                        break;
                    case Level::CellType::OUTFLOW:
                        ff_data.value(x, y) = glm::vec3{ 1.0f, 0.0f, 0.0f }; // red
                        break;
                }
            }
        }
        ff_texture->setData(ff_data);

        // create RenderObject
        RenderObject ff_render_object;
        ff_render_object.mesh = flag_field_mesh.get();
        ff_render_object.position = glm::vec3{0.75f, 0.75f, 0.0f};
        ff_render_object.scale = {0.25, 0.25};
        ff_render_object.rotation = 0;
        ui_objects.push_back(ff_render_object);


        // handle lattice boltzmann output
        assert(latticeBoltzmannOutput.velocity.width() == latticeBoltzmannOutput.density.width());
        assert(latticeBoltzmannOutput.velocity.height() == latticeBoltzmannOutput.density.height());
        fluid_input = Array2D<glm::vec3>{ latticeBoltzmannOutput.velocity.width(), latticeBoltzmannOutput.velocity.height() };
        for (int x = 0; x < fluid_input.width(); ++x)
            for (int y = 0; y < fluid_input.height(); ++y)
                fluid_input.value(x,y) = glm::vec3{ latticeBoltzmannOutput.velocity.value(x,y) * 0.1f + 0.5f, latticeBoltzmannOutput.density.value(x,y) };
        /* test input
        static Array2D<glm::vec2> test_fluid_velocity;
        if (test_fluid_velocity == Array2D<glm::vec2>{}) {
            test_fluid_velocity = Array2D<glm::vec2>{ 42, 42 };
            for (int i = 0; i < test_fluid_velocity.width(); ++i)
                for (int j = 0; j < test_fluid_velocity.height(); ++j)
                    test_fluid_velocity.value(i,j) = {
                        static_cast<float>(i) / test_fluid_velocity.width(),
                        static_cast<float>(j) / test_fluid_velocity.height() };
        }
        fluid_velocity = &test_fluid_velocity;*/


        Log::info("RendererInput: %d objects to render", world_objects.size());
    }
};

#endif
