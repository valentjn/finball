#ifndef GAME_LOGIC_HPP_
#define GAME_LOGIC_HPP_

#include <chrono>

#include <GameLogic/GameLogicInput.hpp>
#include <GameLogic/GameLogicOutput.hpp>
#include <LevelDesign/Level.hpp>
#include <Visualization/RenderObject.hpp>
#include <Visualization/Texture.hpp>
#include <Visualization/Mesh.hpp>

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
	bool showMiniMap;
	steady_clock::time_point startTime;
	const unordered_map<int, Mesh*>& rigid_body_meshes;

public:
	GameLogic(const Level& level, bool showMiniMap);

	void initOutput(GameLogicOutput& output) {}

    void compute(const GameLogicInput &input, GameLogicOutput &output);

    void createInflowMesh(const Level &level);

    void createSharks(const Level &level);
};

#endif
