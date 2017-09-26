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
        // TODO: rename to transform
		for (const Transform* rigidBody : rigidBodyPhysicsOutput.rigid_bodies) {
            auto iter = (*gameLogicOutput.rigid_body_meshes).find(rigidBody->id);
			const Mesh* mesh;
			if (iter != gameLogicOutput.rigid_body_meshes->end()) { // check if the mesh is in the map
				mesh = iter->second;
			}
			else {
				Log::info(
                    "WARNING: rigid body with id %d does not have a render mesh, rendering as square",
                    rigidBody->id);
                mesh = dummy_mesh.get();
            }
            RenderObject renderObject;
            renderObject.position = glm::vec3(rigidBody->position, 0);
            renderObject.scale = glm::vec2{ 1.0f };
            renderObject.rotation = rigidBody->rotation;
			renderObject.mesh = mesh;
            world_objects.push_back(renderObject);
        }

        // visualise the flag field from rigidbody
        static std::unique_ptr<Texture3F> ff_texture;
        static std::unique_ptr<TexturedMesh> ff_mesh;
        const int ff_width = rigidBodyPhysicsOutput.grid_objects.width();
        const int ff_height = rigidBodyPhysicsOutput.grid_objects.height();
        if (!ff_mesh) {
            ff_texture = std::make_unique<Texture3F>(glm::ivec2(ff_width, ff_height), false);
            const std::vector<glm::vec3> flag_field_quad = Mesh::createRectangle(glm::vec2{-1, -1}, glm::vec2{1, 1});
            ff_mesh = std::make_unique<TexturedMesh>(flag_field_quad, ff_texture.get());
        }
        // fill the texture with data from the flag field
        Array2D<glm::vec3> ff_data = Array2D<glm::vec3>(ff_width, ff_height);
        for (int y = 0; y < ff_height; y++) {
            for (int x = 0; x < ff_width; x++) {
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
        ff_render_object.mesh = ff_mesh.get();
        ff_render_object.position = glm::vec3{0.75f, 0.75f, 0.0f};
        ff_render_object.scale = {0.25, 0.25};
        ff_render_object.rotation = 0;
        ui_objects.push_back(ff_render_object);

        // handle lattice boltzmann output
        assert(latticeBoltzmannOutput.velocity.width() == latticeBoltzmannOutput.density.width());
        assert(latticeBoltzmannOutput.velocity.height() == latticeBoltzmannOutput.density.height());
        fluid_input = Array2D<glm::vec3>{
			latticeBoltzmannOutput.velocity.width() - 2, latticeBoltzmannOutput.velocity.height() - 2 };
        for (int x = 0; x < fluid_input.width(); ++x)
            for (int y = 0; y < fluid_input.height(); ++y)
                fluid_input.value(x,y) = glm::vec3{ latticeBoltzmannOutput.velocity.value(x+1,y+1) * 10.f + 0.5f, latticeBoltzmannOutput.density.value(x+1,y+1) * 0.5f };
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
