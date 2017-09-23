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
    Mesh dummyUIMesh;
public:
    std::vector<RenderObject> world_objects;
    std::vector<RenderObject> ui_objects;
    const Array2D<glm::vec2> *fluid_velocity;
    const Array2D<float> *fluid_density;

	/*~RendererInput() {
		//delete dummyUIMesh;
		//delete fluid_velocity;
		//delete fluid_density;
	}*/

    RendererInput() :m_rigid_body_meshes(), dummyUIMesh(createCircleMesh(1.f)), fluid_velocity(nullptr), fluid_density(nullptr) {}

    RendererInput(const GameLogicOutput &gameLogicOutput,
                  const RigidBodyPhysicsOutput &rigidBodyPhysicsOutput,
                  const LatticeBoltzmannOutput &latticeBoltzmannOutput):
                	  m_rigid_body_meshes(),dummyUIMesh(createCircleMesh(1.f)){
        // handle game logic output
        world_objects.insert(world_objects.end(), gameLogicOutput.objectsToRender.begin(),
                             gameLogicOutput.objectsToRender.end());

        for (auto& gameLogicObject : world_objects){
        	if(gameLogicObject.mesh == nullptr){
				Log::error(
						"Error: a game_logic object does not have a render mesh, rendering as circle");
				gameLogicObject.mesh = &dummyUIMesh;
        	}
        }

        Log::info("Rendering %d world_object of gameLogic.",world_objects.size());
        // first generate necessary meshes, if they are not given.
		for (const RigidBody& rigidBody : rigidBodyPhysicsOutput.rigid_bodies) {
			if (m_rigid_body_meshes.count(rigidBody.id) == 0) {
				Log::info(
						"WARNING: rigid body with id %d does not have a render mesh, rendering as circle",
						rigidBody.id);
				m_rigid_body_meshes.emplace(rigidBody.id,createCircleMesh(1.f));
			}
		}

        // handle rigid body physics output
		for (const RigidBody& rigidBody : rigidBodyPhysicsOutput.rigid_bodies) {
            RenderObject renderObject;
            renderObject.position = glm::vec3(rigidBody.position, 0);
            renderObject.scale = rigidBody.radius;
			assert(m_rigid_body_meshes.count(rigidBody.id)!=0);
			renderObject.mesh = &(m_rigid_body_meshes[rigidBody.id]);

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
        Log::info("RendererInput: %d objects to render", world_objects.size());
    }
};

#endif
