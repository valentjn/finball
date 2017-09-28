#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <memory>
#include <unordered_map>
#include <vector>

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
    static const float BALL_RADIUS;
    static const float BALL_MASS;
    static const char *BALL_IMAGE_PATH;
    static const vec3 BALL_COLOR;
    static const vec3 OBSTACLE_COLOR;
    static const vec3 FLIPPER_COLOR;

    unique_ptr<Texture4F> leftFinTexture;
    unique_ptr<Texture4F> rightFinTexture;

    Mesh *cellMesh;

    int width, height;
    int flipperLeftId, flipperRightId;
    Array2D<CellType> matrix;
    vector<unique_ptr<RigidBody>> rigidBodies;
    unordered_map<int, Mesh*> meshes;

    // delete copy constructor and copy-assignment operator
    Level(const Level &) = delete;
    Level &operator=(const Level &) = delete;

    Level();

    void addBall(int id, int x, int y);

    bool isBall(int id) const;

    void setUniqueMesh(int id, unique_ptr<Mesh> mesh);

    Mesh *addUniqueMesh(unique_ptr<Mesh> mesh);

    bool createCellRigidBody(int x, int y, int id);

    void createBoundryRigidBodies(int rigidBodyId);
};

#endif
