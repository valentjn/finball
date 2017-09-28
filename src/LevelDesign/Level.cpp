#include "Level.hpp"

const float Level::FLIPPER_Y = 2.f;
const float Level::FLIPPER_WIDTH = 8.f;
const float Level::FLIPPER_HEIGHT = 3.f;
const float Level::FLIPPER_GAP = 3.5f;

const float Level::BALL_RADIUS = 2.5f;
const float Level::BALL_MASS = 0.5f;
const char *Level::BALL_IMAGE_PATH = "data/ball.png";
const vec3 Level::BALL_COLOR = vec3(1.f, 0, 0);
const vec3 Level::OBSTACLE_COLOR = vec3(0.82f, 0.82f, 0.9f);
const vec3 Level::FLIPPER_COLOR = vec3(1.f, 1.f, 0);

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
