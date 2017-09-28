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
    static const float FLIPPER_Y, FLIPPER_WIDTH, FLIPPER_HEIGHT, FLIPPER_GAP;
    static const float SHARK_Y_OFFSET, SHARK_WIDTH, SHARK_HEIGHT;
    static const float BALL_RADIUS;
    static const float BALL_MASS;
    static const char *BALL_IMAGE_PATH;
    static const vec3 BALL_COLOR, OBSTACLE_COLOR, FLIPPER_COLOR, SHARK_COLOR;

    unique_ptr<Texture4F> leftFinTexture;
    unique_ptr<Texture4F> rightFinTexture;
    unique_ptr<Texture4F> leftSharkTexture;
    unique_ptr<Texture4F> rightSharkTexture;

    Mesh *cellMesh;

    int width, height;
    int flipperLeftId, flipperRightId, sharkLeftId, sharkRightId;
    Array2D<CellType> matrix;
    vector<unique_ptr<RigidBody>> rigidBodies;
    unordered_map<int, Mesh*> meshes;

    void createFlippers();
    void createSharks();

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
            ballMesh = make_unique<TexturedMesh>(
				Mesh::createCircle(vec2(0, 0), BALL_RADIUS),
				ballTexture.get(),
				BALL_RADIUS);
        }
#else
        ballMesh = make_unique<ColoredMesh>(
            Mesh::createCircle(vec2(0, 0), BALL_RADIUS),
            BALL_COLOR
        );
#endif
        cellMesh = addUniqueMesh(make_unique<ColoredMesh>(
           Mesh::createRectangle(vec2(-0.5f, -0.5f), vec2(0.5f, 0.5f)),
           OBSTACLE_COLOR
        ));
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

    bool createCellRigidBody(int x, int y, int id) {
        if (matrix.value(x, y) == CellType::OBSTACLE) {
            rigidBodies.push_back(make_unique<RigidBodyRect>(id, x, y, 1, 1, 0));
            meshes[id] = cellMesh;
            return true;
        }
        return false;
    }

    void createBoundryRigidBodies(int rigidBodyId) {
        int leftX = floor(getLeftFinX());
        int rightX = ceil(getRightFinX());

        if (width > (rightX - leftX + 2)) {
            // for (int x = leftX - 5; x <= rightX + 5; x++) {
            //     matrix.value(x, 0) = CellType::OBSTACLE;
            // }
            for (int x = leftX; x <= rightX; x++) {
                matrix.value(x, 0) = CellType::OUTFLOW;
            }
            for (int y = 0; y <= 4; y++) {
                matrix.value(leftX-1, y) = CellType::OBSTACLE;
                matrix.value(rightX+1, y) = CellType::OBSTACLE;
            }
        }

        for (int x = 0; x < width; x++) {
            if (createCellRigidBody(x, 0, rigidBodyId)) rigidBodyId++;
            if (createCellRigidBody(x, height-1, rigidBodyId)) rigidBodyId++;
        }
        for (int y = 1; y < height-1; y++) {
            if (createCellRigidBody(0, y, rigidBodyId)) rigidBodyId++;
            if (createCellRigidBody(width-1, y, rigidBodyId)) rigidBodyId++;
        }
    }

    float getLeftFinX() const {
        return width * 0.5f - FLIPPER_WIDTH - FLIPPER_GAP;
    }

    float getRightFinX() const {
        return width * 0.5f + FLIPPER_WIDTH + FLIPPER_GAP;
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
