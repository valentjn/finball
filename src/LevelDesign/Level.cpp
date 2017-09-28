#include "Level.hpp"

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

void CollisionType::setValues(CollisionType::Type type)
{
	switch (type) {
		case CollisionType::Type::ELASTIC:
			obsticleElastisity = 1.0f;
			bodyElastisity = 1.0f;
			fluidInfluence = 0.5f;
			break;
		case CollisionType::Type::INELASTIC:
			obsticleElastisity = 0.0f;
			bodyElastisity = 0.0f;
			fluidInfluence = 0.5f;
			break;
		case CollisionType::Type::REALISTIC:
			obsticleElastisity = 0.5f;
			bodyElastisity = 0.5f;
			fluidInfluence = 0.5f;
			break;
		case CollisionType::Type::FLUID_DRIVEN:
			obsticleElastisity = 0.5f;
			bodyElastisity = 0.5f;
			fluidInfluence = 1.0f;
			break;
		default:
			break;
	}
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
