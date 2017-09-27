#include <btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/Gimpact/btTriangleShapeEx.h>
#include <glm/glm.hpp>
#include <math.h>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "LevelDesign/Level.hpp"
#include "Log.hpp"
#include "RigidBody/RigidBodyPhysicsInput.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"
#include "RigidBody/Transform.hpp"

#include "RigidBodyPhysics.hpp"
btHingeAccumulatedAngleConstraint *hingeC;
void RigidBodyPhysics::addRigidBody(const unique_ptr <RigidBody> &level_body)
{
	btRigidBody *bt_rigid_body = createBtRigidBody(level_body);

	// Constrain to two dimensions
	bt_rigid_body->setLinearFactor(btVector3(1.f, 1.f, 0.f));
	bt_rigid_body->setAngularFactor(btVector3(0.f, 0.f, 1.f));

	auto rigid_body =
			std::make_unique<Transform>(level_body->id, level_body->position, level_body->rotation);
	rigid_bodies[level_body->id] = std::move(rigid_body);

	// bt_rigid_body->setUserIndex2(static_cast<int>(detection_type));
	dynamics_world->addRigidBody(bt_rigid_body);

	if (level_body->id == level.flipperLeftId)
	{
		bt_rigid_body->setActivationState(DISABLE_DEACTIVATION);
		hingeC = new btHingeAccumulatedAngleConstraint(*bt_rigid_body,btVector3(0,0,0),btVector3(0,0,1));
		hingeC->setLimit(0, SIMD_PI/4);
		dynamics_world->addConstraint(hingeC);
	}

	if (level_body->mass == 0.f) {
		// TODO: do this more accurately?
		// TODO: this only works for rectangles atm
		// FIXME: take width and height into consideration
		grid_static_objects_flow.value(level_body->position.x, level_body->position.y) = Level::CellType::OBSTACLE;
	}
}

// Add one rigid body that is invisible to user at an inflow cell
void RigidBodyPhysics::createBoundaryRigidBody(btCollisionShape *collision_shape,
											   btDefaultMotionState *motion_state, btRigidBody *bt_rigid_body,
											   btTransform &transform, const int x, const int y)
{

	btVector3 extents = DISTANCE_GRID_CELLS * btVector3(0.5f, 0.5f, 0.0f); // One grid cell
	collision_shape = new btBoxShape(extents);
	transform.setOrigin(DISTANCE_GRID_CELLS * btVector3(x, y, 0.0f)); // grid center
	motion_state = new btDefaultMotionState(transform);
	bt_rigid_body = new btRigidBody(0.0f, motion_state, collision_shape, btVector3(0.0f, 0.0f, 0.0f));
	bt_rigid_body->setUserIndex(-1); // Set user index to -1 to distinguish from obstacles
	dynamics_world->addRigidBody(bt_rigid_body);
}

// TODO: Instead of multiple rigid bodies just make a longer rectangle
// Add rigid bodies invisible to the user at inflow cells
void RigidBodyPhysics::addBoundaryRigidBodies()
{
	btCollisionShape *collision_shape;
	btDefaultMotionState *motion_state;
	btRigidBody *bt_rigid_body;
	btTransform transform;
	transform.setIdentity();
	int x = 0, y = 0;
	// Use two for loops as we only want to iterate over the wall
	// Left and right walls
	for (y = 0; y < grid_static_objects_flow.height(); ++y) {
		// Left wall
		x = 0;
		if (grid_static_objects_flow.value(x, y) == Level::CellType::INFLOW) {
			createBoundaryRigidBody(collision_shape, motion_state, bt_rigid_body, transform, x, y);
		}
		// Right wall
		x = grid_static_objects_flow.width() - 1;
		if (grid_static_objects_flow.value(x, y) == Level::CellType::INFLOW) {
			createBoundaryRigidBody(collision_shape, motion_state, bt_rigid_body, transform, x, y);
		}
	}
	// Top and bottom
	for (x = 0; x < grid_static_objects_flow.width(); ++x) {
		// Bottom wall
		y = 0;
		if (grid_static_objects_flow.value(x, y) == Level::CellType::INFLOW) {
			createBoundaryRigidBody(collision_shape, motion_state, bt_rigid_body, transform, x, y);
		}
		// Top wall
		y = grid_static_objects_flow.height() - 1;
		if (grid_static_objects_flow.value(x, y) == Level::CellType::INFLOW) {
			createBoundaryRigidBody(collision_shape, motion_state, bt_rigid_body, transform, x, y);
		}
	}
}

btRigidBody* RigidBodyPhysics::createBtRigidBody(const unique_ptr <RigidBody> &level_body)
{
	btCollisionShape *collision_shape;
	if (typeid(*level_body) == typeid(RigidBodyCircle)) {
		RigidBodyCircle *circle = static_cast<RigidBodyCircle *>(level_body.get());
		collision_shape = new btSphereShape(circle->radius * DISTANCE_GRID_CELLS); // FIXME: Memory leak
	} else if (typeid(*level_body) == typeid(RigidBodyRect)) {
		RigidBodyRect *rectangle = static_cast<RigidBodyRect *>(level_body.get());
		collision_shape = new btBoxShape(btVector3(rectangle->width / 2., rectangle->height / 2., 0) *
										 DISTANCE_GRID_CELLS); // these are half-extents!
		collision_shape = default_collision_shape.get(); // FIXME: Memory leak
	} else if (typeid(*level_body) == typeid(RigidBodyTriangle)) {
		RigidBodyTriangle *triangle = static_cast<RigidBodyTriangle *>(level_body.get());
		btVector3 p0 = btVector3(triangle->points[0].x, triangle->points[0].y, 0.) * DISTANCE_GRID_CELLS;
		btVector3 p1 = btVector3(triangle->points[1].x, triangle->points[1].y, 0.) * DISTANCE_GRID_CELLS;
		btVector3 p2 = btVector3(triangle->points[2].x, triangle->points[2].y, 0.) * DISTANCE_GRID_CELLS;
		collision_shape = new btTriangleShapeEx(p0, p1, p2); // FIXME: Memory leak
	} else {
		Log::error("RigidBody didn't have a specific shape! Creating a default sphere.");
		collision_shape = default_collision_shape.get();
	}

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(level_body->position.x * DISTANCE_GRID_CELLS,
								  level_body->position.y * DISTANCE_GRID_CELLS, 0.f));
	btDefaultMotionState *motion_state = new btDefaultMotionState(transform);
	btScalar mass = level_body->mass;
	btVector3 inertia;
	collision_shape->calculateLocalInertia(mass, inertia);

	btRigidBody *bt_rigid_body = new btRigidBody(mass, motion_state, collision_shape, inertia);
	bt_rigid_body->setUserIndex(level_body->id);
	return bt_rigid_body;
}

// TODO: dtor

float RigidBodyPhysics::gridToBullet(int x)
{
	return static_cast<float>(x) * DISTANCE_GRID_CELLS;
}

glm::vec2 RigidBodyPhysics::gridToBullet(int x, int y)
{
	return glm::vec2(gridToBullet(x),
					 gridToBullet(y));
}

//glm::vec2 gridToBullet(const glm::vec2& vec) {
//    return gridToBullet(vec. );
//}

float RigidBodyPhysics::bulletToGrid(float x)
{
	return x * DISTANCE_GRID_CELLS_INV;
}

glm::vec2 RigidBodyPhysics::bulletToGrid(float x, float y)
{
	return glm::vec2(bulletToGrid(x), bulletToGrid(y));
}

void RigidBodyPhysics::clearDynamicFlagFields(Array2D < Level::CellType > &grid_obj)
{
	for (int y = 0; y < GRID_HEIGHT; ++y) {
		for (int x = 0; x < GRID_WIDTH; ++x) {
			grid_ball.value(x, y) = Level::CellType::FLUID;
			grid_obj.value(x, y) = grid_static_objects_flow.value(x, y);
			// grid_pedals.value(x, y) = false; // TODO
			grid_velocities.value(x, y) = glm::vec2{0., 0.};
		}
	}
}

void RigidBodyPhysics::markRBAsObstacles(Array2D < Level::CellType > &grid_obj)
{
	for (int y = GRID_HEIGHT - 1; y >= 0; --y) {
		for (int x = 0; x < GRID_WIDTH; ++x) {
			// TODO: grid_pedals

			if (grid_ball.value(x, y) == Level::CellType::OBSTACLE) {
				grid_obj.value(x, y) = Level::CellType::OBSTACLE;
			}
		}
	}
}

void RigidBodyPhysics::markBallAsObstacle(int id, Array2D <glm::vec2> &grid_vel, btRigidBody *bt_rigid_body,
										  btVector3 &origin)
{
	for (int y = 0; y < GRID_HEIGHT; ++y) {
		for (int x = 0; x < GRID_WIDTH; ++x) {
			glm::vec2 pos = gridToBullet(x, y);
			if (glm::distance(pos, glm::vec2{origin.getX(), origin.getY()}) <=
				gridToBullet(static_cast<RigidBodyCircle *>(level.rigidBodies[id - 1].get())->radius)) {
				grid_ball.value(x, y) = Level::CellType::OBSTACLE;
				// TODO: Proper scaling of velocity. Right now only scale the length.
				grid_vel.value(x, y) = gridToBullet(bt_rigid_body->getLinearVelocity().x(),
													bt_rigid_body->getLinearVelocity().y());
			}
		}
	}
}

void RigidBodyPhysics::applyImpulses(btCollisionObject *&obj)
{
	btRigidBody *bt_rigid_body = btRigidBody::upcast(obj);
	int id = bt_rigid_body->getUserIndex();
	Transform *rigid_body = rigid_bodies[id].get();
	if (bt_rigid_body && bt_rigid_body->getMotionState() && rigid_body->id == Level::BALL_ID) {
		bt_rigid_body->applyCentralImpulse(DISTANCE_GRID_CELLS * btVector3(impulses[1].x, impulses[1].y, 0.0f));
	}
}

//TODO give more meaningful name
void RigidBodyPhysics::processRigidBody(btCollisionObject *&obj, RigidBodyPhysicsOutput &output,
										Array2D <glm::vec2> &grid_vel)
{
	btRigidBody *bt_rigid_body = btRigidBody::upcast(obj);
	btTransform transform;
	if (bt_rigid_body && bt_rigid_body->getMotionState()) {
		bt_rigid_body->getMotionState()->getWorldTransform(transform);
		btVector3 & origin = transform.getOrigin();

		// update the RigidBody and push the reference to our output
		int id = bt_rigid_body->getUserIndex();
		Transform *output_transform = rigid_bodies[id].get();

		output_transform->position.x = bulletToGrid(origin.getX());
		output_transform->position.y = bulletToGrid(origin.getY());

		// TODO: check that this behaves correctly
		// auto quaternion = transform.getRotation();
		// if (quaternion.getAxis().z() < 0.) {
		// 	output_transform->rotation = 2 * M_PI - quaternion.getAngle();
		// } else {
		// 	output_transform->rotation = quaternion.getAngle();
		// }


		if (id == level.flipperLeftId) {
			printf("%f \t %f\n",output_transform->rotation, hingeC->getAccumulatedHingeAngle());
		}

		output.rigid_bodies.push_back(output_transform);

		if (id == Level::BALL_ID) {
			markBallAsObstacle(id, grid_vel, bt_rigid_body, origin);
		}
	} else {
		Log::error("Problem in RigidBodyPhysics compute(): no RigidBody or could not get MotionState");
		throw std::runtime_error("Problem in RigidBodyPhysics compute(): no RigidBody or could not get MotionState");
	}
}


void RigidBodyPhysics::compute(const RigidBodyPhysicsInput &input, RigidBodyPhysicsOutput &output)
{
	auto &grid_obj = output.grid_objects;
	auto &grid_vel = output.grid_velocities;
	output.rigid_bodies.clear();
	// TODO: try to give out a const reference to our internal rigid_bodies vector

	// Compute impulses
	// TODO: Make it work for multiple balls.
	input.computeImpulses(grid_ball, impulses);

	for (int j = 0; j < dynamics_world->getNumCollisionObjects(); j++) {
		auto &obj = dynamics_world->getCollisionObjectArray()[j];
		if (obj->getUserIndex() == -1) // Ignore the inflow boundary objects
			continue;
		applyImpulses(obj);
	}

	// Clear the impulses for the next time step
	impulses.clear();

	clearDynamicFlagFields(grid_obj);
	hingeC->enableAngularMotor(true,1.0f,SIMD_INFINITY);
	dynamics_world->stepSimulation(1. / 60.); // TODO: everybody has to use the same timestep

	for (int j = 0; j < dynamics_world->getNumCollisionObjects(); j++) {
		auto &obj = dynamics_world->getCollisionObjectArray()[j];
		if (obj->getUserIndex() == -1) // Ignore the inflow boundary objects
			continue;
		processRigidBody(obj, output, grid_vel);
	}

	// TODO: mesh tohether the individual flag fields
	markRBAsObstacles(grid_obj);
	for (int j = 0; j < dynamics_world->getNumCollisionObjects(); j++) {
		auto &obj = dynamics_world->getCollisionObjectArray()[j];
		btRigidBody *rigid_body = btRigidBody::upcast(obj);
		if (rigid_body && (obj->getUserIndex() == level.flipperLeftId || obj->getUserIndex() == level.flipperRightId)) {
			grid_finFlag(grid_obj, grid_vel, rigid_body);
		} else {
			std::runtime_error("Fin is not a btRigidBody!");
		}
	}

	// for (int i = GRID_HEIGHT - 1; i >= 0; --i) {
	//     for (int j = 0; j < GRID_WIDTH; ++j) {
	//         printf("%2d", grid_obj.value(j, i));
	//     }
	//     printf("\n");
	// }
	// printf("-----------\n");

	// TODO: determine which cells are occupied by obj
	// grid_obj.value(1, 3) = Level::CellType::FLUID;
	// grid_vel.value(1, 3) = glm::vec2{1.0, 0.5};
}

void RigidBodyPhysics::setGravity(bool on)
{
	dynamics_world->setGravity(btVector3(0., (on ? -10. : 0.), 0.));
}

Transform RigidBodyPhysics::getRigidBody(int idx)
{
	return *(rigid_bodies[idx].get());
}

void RigidBodyPhysics::setRigidBody(int id, float x, float y, float mass = 1.f, float rotation = 0.f)
{
	//loop over the ones already there

	//create a new one
}

void RigidBodyPhysics::grid_finFlag(Array2D < Level::CellType > &grid_fin, Array2D < glm::vec2 > &grid_vel, btRigidBody
																											*
																											rigid_body)
{
	btTransform transform;
	rigid_body->getMotionState()->getWorldTransform(transform);
	// TODO: use typeid to check that the shape is correct? (col_shape.getShapeType())
	const btTriangleShapeEx *triangle_shape = static_cast<const btTriangleShapeEx *>(rigid_body->getCollisionShape());

	const btVector3 &p1 = triangle_shape->getVertexPtr(0);
	const btVector3 &p2 = triangle_shape->getVertexPtr(1);
	const btVector3 &p3 = triangle_shape->getVertexPtr(2);

	btVector3 p1_world = transform * p1;
	btVector3 p2_world = transform * p2;
	btVector3 p3_world = transform * p3;

	glm::vec2 pos1 = {p1_world.x(), p1_world.y()};
	glm::vec2 pos2 = {p2_world.x(), p2_world.y()};
	glm::vec2 pos3 = {p3_world.x(), p3_world.y()};

	// // Array2D<glm::vec2>& grid_vel, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3
	glm::vec2 norm1(-(pos1.y - pos2.y), pos1.x - pos2.x);
	glm::vec2 norm2(-(pos2.y - pos3.y), pos2.x - pos3.x);
	glm::vec2 norm3(-(pos3.y - pos1.y), pos3.x - pos1.x);

	// TODO: only check in the AABB
	for (int i = 0; i < grid_fin.width(); i++) {
		for (int j = 0; j < grid_fin.height(); j++) {
			glm::vec2 tempVec1 = gridToBullet(i, j) - pos1;
			glm::vec2 tempVec2 = gridToBullet(i, j) - pos2;
			glm::vec2 tempVec3 = gridToBullet(i, j) - pos3;
			if ((tempVec1.x * norm1.x + tempVec1.y * norm1.y >= 0) &&
				(tempVec2.x * norm2.x + tempVec2.y * norm2.y >= 0) &&
				(tempVec3.x * norm3.x + tempVec3.y * norm3.y >= 0)) {
				grid_fin.value(i, j) = Level::CellType::OBSTACLE;
				grid_vel.value(i, j) = glm::vec2(0., 0.); // TODO
			}
		}
	}
}
