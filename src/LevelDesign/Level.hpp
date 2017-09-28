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

    Level();

    void addBall(int id, int x, int y);

    bool isBall(int id) const;

    void setUniqueMesh(int id, unique_ptr<Mesh> mesh);

    Mesh *addUniqueMesh(unique_ptr<Mesh> mesh);

    bool createCellRigidBody(int x, int y, int id);

    void createBoundryRigidBodies(int rigidBodyId);

    float getLeftFinX() const;

    float getRightFinX() const;
};

#endif
