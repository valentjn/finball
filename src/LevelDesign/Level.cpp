#ifdef OPENCV_LIBS
#include <opencv2/opencv.hpp>
#endif

#include <Log.hpp>
#include <LevelDesign/Level.hpp>

const float Level::FLIPPER_Y = 5.f;
const float Level::FLIPPER_WIDTH = 8.f;
const float Level::FLIPPER_HEIGHT = 3.f;
const float Level::FLIPPER_GAP = 3.5f;

const float Level::SHARK_Y_OFFSET = -4.f;
const float Level::SHARK_WIDTH = 7.f;
const float Level::SHARK_HEIGHT = 14.f;

const float Level::BALL_RADIUS = 2.5f;

const float Level::BALL_MASS = 0.2f;

const char *Level::BALL_IMAGE_PATH = "data/ball.png";
const vec3 Level::BALL_COLOR = vec3(1.f, 0, 0);
const vec3 Level::OBSTACLE_COLOR = vec3(0.82f, 0.82f, 0.9f);
const vec3 Level::FLIPPER_COLOR = vec3(1.f, 1.f, 0);
const vec3 Level::SHARK_COLOR = vec3(0.f, 0.f, 1.f);

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
		ballMesh = make_unique<TexturedMesh>(
			Mesh::createCircle(vec2(0, 0), BALL_RADIUS), ballTexture.get(), BALL_RADIUS);
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

void Level::createFlippers() {
	// positive masses as they aren't static objects
	auto rigidBodyLeft = make_unique<RigidBodyTriangle>(flipperLeftId, getLeftFinX(), FLIPPER_Y,
														vec2(FLIPPER_WIDTH, -FLIPPER_HEIGHT),
														vec2(0.f, -FLIPPER_HEIGHT),
														10);
	auto rigidBodyRight = make_unique<RigidBodyTriangle>(flipperRightId, getRightFinX(), FLIPPER_Y,
														 vec2(0.f, -FLIPPER_HEIGHT),
														 vec2(-FLIPPER_WIDTH, -FLIPPER_HEIGHT),
														 10);
#ifdef OPENCV_LIBS
	std::unique_ptr<Mesh> meshLeft, meshRight;
	try {
		Texture4F::createImage(leftFinTexture, "data/fin_left.png");
		glm::vec2 p2 = glm::vec2(leftFinTexture->size());
		p2 *= FLIPPER_WIDTH / p2.x;
		glm::vec2 p1 = -p2;
		auto rectangle = Mesh::createRectangle(p1, p2);
		meshLeft = std::make_unique<TexturedMesh>(rectangle, leftFinTexture.get(), p1, p2);
	}
	catch (const std::runtime_error&) {
		meshLeft = rigidBodyLeft->createColoredMesh(FLIPPER_COLOR);
	}
	try {
		Texture4F::createImage(rightFinTexture, "data/fin_right.png");
		glm::vec2 p2 = glm::vec2(rightFinTexture->size());
		p2 *= FLIPPER_WIDTH / p2.x;
		glm::vec2 p1 = -p2;
		auto rectangle = Mesh::createRectangle(p1, p2);
		meshRight = std::make_unique<TexturedMesh>(rectangle, rightFinTexture.get(), p1, p2);
	}
	catch (const std::runtime_error&) {
		meshRight = rigidBodyRight->createColoredMesh(FLIPPER_COLOR);
	}
	setUniqueMesh(flipperLeftId, std::move(meshLeft));
	setUniqueMesh(flipperRightId, std::move(meshRight));
#else
	setUniqueMesh(flipperLeftId, rigidBodyLeft->createColoredMesh(FLIPPER_COLOR));
	setUniqueMesh(flipperRightId, rigidBodyRight->createColoredMesh(FLIPPER_COLOR));
#endif

	rigidBodies.push_back(std::move(rigidBodyLeft));
	rigidBodies.push_back(std::move(rigidBodyRight));
}

void Level::createSharks() {
	auto rigidBodyLeft = make_unique<RigidBodyTriangle>(sharkLeftId, getLeftFinX(), FLIPPER_Y,
														vec2( SHARK_WIDTH/2, SHARK_Y_OFFSET),
														vec2(-SHARK_WIDTH/2, SHARK_Y_OFFSET),
														vec2(          0.0f, SHARK_Y_OFFSET + SHARK_HEIGHT),
														0);
	auto rigidBodyRight = make_unique<RigidBodyTriangle>(sharkRightId, getRightFinX(), FLIPPER_Y,
														 vec2(-SHARK_WIDTH/2, SHARK_Y_OFFSET),
														 vec2( SHARK_WIDTH/2, SHARK_Y_OFFSET),
														 vec2(          0.0f, SHARK_Y_OFFSET + SHARK_HEIGHT),
														 0);

#ifdef OPENCV_LIBS
	std::unique_ptr<Mesh> meshLeft, meshRight;
	try {
		Texture4F::createImage(leftSharkTexture, "data/shark_left.png");
		glm::vec2 p2 = glm::vec2(leftSharkTexture->size());
		p2 *= SHARK_HEIGHT / p2.y;
		glm::vec2 p1 = -p2;
		auto rectangle = Mesh::createRectangle(p1, p2, -0.5f);
		meshLeft = std::make_unique<TexturedMesh>(rectangle, leftSharkTexture.get(), p1, p2);
	}
	catch (const std::runtime_error&) {
		meshLeft = rigidBodyLeft->createColoredMesh(SHARK_COLOR);
	}
	try {
		Texture4F::createImage(rightSharkTexture, "data/shark_right.png");
		glm::vec2 p2 = glm::vec2(rightSharkTexture->size());
		p2 *= SHARK_HEIGHT / p2.y;
		glm::vec2 p1 = -p2;
		auto rectangle = Mesh::createRectangle(p1, p2, -0.5f);
		meshRight = std::make_unique<TexturedMesh>(rectangle, rightSharkTexture.get(), p1, p2);
	}
	catch (const std::runtime_error&) {
		meshRight = rigidBodyRight->createColoredMesh(SHARK_COLOR);
	}
	setUniqueMesh(sharkLeftId, move(meshLeft));
	setUniqueMesh(sharkRightId, move(meshRight));
#else
	setUniqueMesh(sharkLeftId, rigidBodyLeft->createColoredMesh(SHARK_COLOR));
	setUniqueMesh(sharkRightId, rigidBodyRight->createColoredMesh(SHARK_COLOR));
#endif

	rigidBodies.push_back(std::move(rigidBodyLeft));
	rigidBodies.push_back(std::move(rigidBodyRight));
}
