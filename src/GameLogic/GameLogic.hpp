#ifndef GAME_LOGIC_HPP_
#define GAME_LOGIC_HPP_

#include <chrono>
#include <fstream>
#include <glm/glm.hpp>
#include <vector>

#ifdef OPENCV_LIBS
#include <opencv2/opencv.hpp>
#endif

#include "GameLogic/GameLogicInput.hpp"
#include "GameLogic/GameLogicOutput.hpp"
#include "LevelDesign/Level.hpp"
#include "LevelDesign/LevelLoader.hpp"
#include "Log.hpp"
#include "Visualization/RenderObject.hpp"
#include "Visualization/Texture.hpp"
#include "Visualization/Mesh.hpp"

using namespace std;
using namespace chrono;

class GameLogic
{
private:
	unique_ptr<TexturedMesh> fluid_mesh;
	unique_ptr<ColoredMesh> inflow_mesh;
	RenderObject fluid_surface;
	RenderObject inflow_surface;

	const Level &level;
	steady_clock::time_point startTime;
	const unordered_map<int, Mesh*>& rigid_body_meshes;

public:
	GameLogic(const Level& level)
		: level(level), rigid_body_meshes(level.meshes)
	{
		auto rectangle = Mesh::createRectangle({ -1.f, -1.f }, { 1.f, 1.f });
		fluid_mesh = make_unique<TexturedMesh>(rectangle, nullptr);
		fluid_surface.position = { (level.width - 1) * 0.5f, (level.height - 1) * 0.5f, 0.2f };
		fluid_surface.rotation = 0;
		fluid_surface.scale = { (level.width - 2) * 0.5f, (level.height - 2) * 0.5f };
		fluid_surface.mesh = fluid_mesh.get();

		createInflowMesh(level);

		startTime = steady_clock::now();
		Log::debug("Haiscore clock started ;-)");
	}

	void initOutput(GameLogicOutput& output) {}

    void compute(const GameLogicInput &input, GameLogicOutput &output) {
        duration<float> duration = steady_clock::now() - startTime;
        output.score = duration.count();

		bool foundBall = false;
        bool allBallsOutside = true;
        for (Transform const *rigidBody : *input.rigidBodies) {
            if (level.isBall(rigidBody->id)) {
				foundBall = true;
				if (rigidBody->position.y >= 0 && rigidBody->position.y <= level.height &&
						 rigidBody->position.x >= 0 && rigidBody->position.x <= level.width) {
	                allBallsOutside = false;
					break;
				}
			}
        }

        if (input.quit || (allBallsOutside && foundBall)) {
            output.running = false;
            return;
        }

        if (output.objectsToRender.empty()) {
            output.objectsToRender.push_back(fluid_surface);
            output.objectsToRender.push_back(inflow_surface);
        }

        output.fluid_mesh = fluid_mesh.get();
		output.rigid_body_meshes = &rigid_body_meshes;
    }

    void createInflowMesh(const Level &level) {
        vector<glm::vec3> verticies;
        vector<glm::vec3> rect1 = Mesh::createRectangle({0,0}, {1,level.height});
        vector<glm::vec3> rect2 = Mesh::createRectangle({1,level.height-1}, {level.width-1,level.height});
        vector<glm::vec3> rect3 = Mesh::createRectangle({level.width-1,0}, {level.width,level.height});
        vector<glm::vec3> rect4 = Mesh::createRectangle({1,0}, {level.getLeftFinX(),1});
        vector<glm::vec3> rect5 = Mesh::createRectangle({level.getRightFinX(),0}, {level.width-1,1});
        verticies.insert(verticies.end(), rect1.begin(), rect1.end());
        verticies.insert(verticies.end(), rect2.begin(), rect2.end());
        verticies.insert(verticies.end(), rect3.begin(), rect3.end());
        verticies.insert(verticies.end(), rect4.begin(), rect4.end());
        verticies.insert(verticies.end(), rect5.begin(), rect5.end());
        inflow_mesh = make_unique<ColoredMesh>(verticies, glm::vec3{0.f, 1.f, 0.f});

        inflow_surface.mesh = inflow_mesh.get();
        inflow_surface.position = {-0.5f, -0.5f, 0.1f};
        inflow_surface.rotation = 0;
        inflow_surface.scale = {1.f, 1.f};
    }
};

#endif
