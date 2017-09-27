#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <memory>
#include <unordered_map>
#include <vector>
#include <iostream>

#ifdef OPENCV_LIBS
#include <opencv2/opencv.hpp>
#endif

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
    vector<int> ballIds;

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

    void addBall(int id) {
        ballIds.push_back(id);
        rigidBodies.push_back(make_unique<RigidBodyCircle>(id, x, y, BALL_RADIUS, BALL_MASS);

#ifdef OPENCV_LIBS
        cv::Mat img = cv::imread(BALL_IMAGE_PATH, cv::IMREAD_UNCHANGED);
        if (!img.data) {
            Log::warn("Ball image not found! Using default color mesh.");
            level.setUniqueMesh(Level::BALL_ID, rigidBody->createColoredMesh(BALL_COLOR));
            return;;
        }
        Texture4F *texture = addUniqueTexture(make_unique<Texture4F>(glm::ivec2{img.cols, img.rows}));
        texture->setData(img);
        setUniqueMesh(id, make_unique<TexturedMesh>(3, texture));
#else
        setUniqueMesh(id, rigidBody->createColoredMesh(BALL_COLOR));
#endif
    }

    bool isBall(int id) {
        for (const int &i : ballIds) {
            if (i == id) {
                return true;
            }
        }
        return false;
    }

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
