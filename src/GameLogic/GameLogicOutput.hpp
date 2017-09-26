#ifndef GAME_LOGIC_OUTPUT_HPP_
#define GAME_LOGIC_OUTPUT_HPP_

#include <vector>

#include "Visualization/RenderObject.hpp"

class GameLogicOutput {
public:
    bool running;
    float highscore;
    std::vector<RenderObject> objectsToRender;
    TexturedMesh* fluid_mesh;
	const std::unordered_map<int, Mesh*>* rigid_body_meshes;

    GameLogicOutput() : running(true), highscore(0) {}
};

#endif
