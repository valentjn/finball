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
    unique_ptr<Texture4F> ballTexture;
    unique_ptr<Mesh> ballMesh;
    vector<int> ballIds;

public:
    enum CellType { FLUID, OBSTACLE, INFLOW, OUTFLOW };
    static const float BALL_RADIUS;
    static const float BALL_MASS;
    static const char *BALL_IMAGE_PATH;
    static const vec3 BALL_COLOR;
    static const vec3 OBSTACLE_COLOR;
    static const vec3 FLIPPER_COLOR;

    unique_ptr<Texture4F> leftFinTexture;
    unique_ptr<Texture4F> rightFinTexture;
    
    int width, height;
    int flipperLeftId, flipperRightId;
    Array2D<CellType> matrix;
    vector<unique_ptr<RigidBody>> rigidBodies;
    unordered_map<int, Mesh*> meshes;

    // delete copy constructor and copy-assignment operator
    Level(const Level &) = delete;
    Level &operator=(const Level &) = delete;

    Level() {
#ifdef OPENCV_LIBS
        cv::Mat img = cv::imread(BALL_IMAGE_PATH, cv::IMREAD_UNCHANGED);
        if (!img.data) {
            Log::warn("Ball image not found! Using default color mesh.");
            ballMesh = make_unique<ColoredMesh>(
                Mesh::createCircle(vec2(0, 0), BALL_RADIUS),
                BALL_COLOR
            );
        }
        else {
            ballTexture = make_unique<Texture4F>(glm::ivec2{img.cols, img.rows});
            ballTexture->setData(img);
            ballMesh = make_unique<TexturedMesh>(static_cast<int>(ceil(BALL_RADIUS)), ballTexture.get());
        }
#else
        ballMesh = make_unique<ColoredMesh>(
            Mesh::createCircle(vec2(0, 0), BALL_RADIUS),
            BALL_COLOR
        );
#endif
    }

    void addBall(int id, int x, int y) {
        ballIds.push_back(id);
        meshes[id] = ballMesh.get();
        rigidBodies.push_back(make_unique<RigidBodyCircle>(id, x, y, BALL_RADIUS, BALL_MASS));
    }

    bool isBall(int id) const {
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
