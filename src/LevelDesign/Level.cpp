#ifdef OPENCV_LIBS
#include <opencv2/opencv.hpp>
#endif

#include <Log.hpp>
#include <LevelDesign/Level.hpp>

const float Level::FLIPPER_Y = 5.f;
const float Level::FLIPPER_WIDTH = 8.f;
const float Level::FLIPPER_HEIGHT = 3.f;
const float Level::FLIPPER_GAP = 3.5f;

const float Level::BALL_RADIUS = 2.5f;

const float Level::BALL_MASS = 0.2f;

const char *Level::BALL_IMAGE_PATH = "data/ball.png";
const vec3 Level::BALL_COLOR = vec3(1.f, 0, 0);
const vec3 Level::OBSTACLE_COLOR = vec3(0.82f, 0.82f, 0.9f);
const vec3 Level::FLIPPER_COLOR = vec3(1.f, 1.f, 0);

Level::Level() {
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
	cellMesh = addUniqueMesh(make_unique<ColoredMesh>(
	   Mesh::createRectangle(vec2(-0.5f, -0.5f), vec2(0.5f, 0.5f)),
	   OBSTACLE_COLOR
	));
}

void Level::addBall(int id, int x, int y) {
    ballIds.push_back(id);
    meshes[id] = ballMesh.get();
    rigidBodies.push_back(make_unique<RigidBodyCircle>(id, x, y, BALL_RADIUS, BALL_MASS));
}

bool Level::isBall(int id) const {
    for (const int &i : ballIds) {
        if (i == id) {
            return true;
        }
    }
    return false;
}

void Level::setUniqueMesh(int id, unique_ptr<Mesh> mesh) {
	meshes[id] = mesh.get();
    unique_meshes.push_back(move(mesh));
}

Mesh *Level::addUniqueMesh(unique_ptr<Mesh> mesh) {
	auto ret = mesh.get();
    unique_meshes.push_back(move(mesh));
    return ret;
}

bool Level::createCellRigidBody(int x, int y, int id) {
    if (matrix.value(x, y) == CellType::OBSTACLE) {
        rigidBodies.push_back(make_unique<RigidBodyRect>(id, x, y, 1, 1, 0));
        meshes[id] = cellMesh;
        return true;
    }
    return false;
}

void Level::createBoundryRigidBodies(int rigidBodyId) {
    int leftX = floor(getLeftFinX());
    int rightX = ceil(getRightFinX());

    if (width > (rightX - leftX + 2)) {
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

float Level::getLeftFinX() const {
    return width * 0.5f - FLIPPER_WIDTH - FLIPPER_GAP;
}

float Level::getRightFinX() const {
    return width * 0.5f + FLIPPER_WIDTH + FLIPPER_GAP;
}
