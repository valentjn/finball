#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <memory>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "Log.hpp"
#include "Array2D.hpp"
#include "RigidBody/RigidBody.hpp"
#include "Visualization/Mesh.hpp"
#include "Visualization/Texture.hpp"

using namespace std;
using namespace glm;

struct Level {
private:
    vector<unique_ptr<Mesh>> unique_meshes;
    vector<unique_ptr<Texture4F>> unique_textures;

public:
    enum CellType { FLUID, OBSTACLE, INFLOW, OUTFLOW };
    static const int BALL_ID;
    static const float BALL_RADIUS;
    static const float BALL_MASS;
    static const char *BALL_IMAGE_PATH;
    static const vec3 BALL_COLOR;
    static const vec3 OBSTACLE_COLOR;
    static const vec3 FLIPPER_COLOR;

    int width, height;
    int flipperLeftId, flipperRightId;
    Array2D<CellType> matrix;
    vector<unique_ptr<RigidBody>> rigidBodies;
    unordered_map<int, Mesh*> meshes;

    // delete copy constructor and copy-assignment operator
    Level(const Level &) = delete;
    Level &operator=(const Level &) = delete;

    Level() {}

    void setUniqueMesh(int id, unique_ptr<Mesh> mesh) {
		meshes[id] = mesh.get();
        unique_meshes.push_back(move(mesh));
    }

    Mesh *addUniqueMesh(unique_ptr<Mesh> mesh) {
		auto ret = mesh.get();
        unique_meshes.push_back(move(mesh));
        return ret;
    }

    Texture4F *addUniqueTexture(unique_ptr<Texture4F> texture) {
		auto ret = texture.get();
        unique_textures.push_back(move(texture));
        return ret;
    }
};

struct CollisionType
{
	enum Type { ELASTIC, INELASTIC, REALISTIC, FLUID_DRIVEN };
	Type type;

	float obsticleElastisity;
	float bodyElastisity;
	float fluidInfluence;

	CollisionType(Type type) {
		this->type = type;
		setValues(type);
	}

private:
	void setValues(Type type);

};

#endif
