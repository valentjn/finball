#ifndef RIGID_BODY_PHYSICS_HPP_
#define RIGID_BODY_PHYSICS_HPP_

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <math.h>
#include <memory>
#include <unordered_map>

#include "LevelDesign/Level.hpp"
#include "Log.hpp"
#include "RigidBody/RigidBodyPhysicsInput.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"
#include "RigidBody/Transform.hpp"

// TODO: Maybe name variables properly??
// To distinguish bullet unit variables from grid unit variables

// GET THE BODY SHAPE RESPONSIBILITY THING SORTED OUT!!!! (global id for each object)
// then only send out transform related to the objects with that id (rename RigidBody to Transform)
// (convince others to use r_ball = GRID_WIDTH / 16 / 2)
// use applyCentralImpulse (ignore torque)

// (try to disallow id == 0 so we can thn) (still tyu this for other optimizations))
// (0 0 0 00)
// (0 0-1 00)
// (0-1 1-10)
// (0 0-1 00)
// (00000000)
// where 1 is the body and -1 the surrounding cells (only for dynamic objects (ball; pedals))
// -> impulse is applied there (where? negative field?; positive field? what if there are
// two/three???)
// (for static objs we don't beed the adjacent cells as no force applies on them anyway)

// different set of flagfields for static objs, ball, ...? (ball is just clear everything and mark
// cells where dist <= radius; static just stays the same (id in the field for easy removal; needs
// to be converted to CellType))
// pedals will need either ghost objects or triangle rendering (depends on exact shape)
// ghost objects only in that bottom area?

class RigidBodyPhysics {
private:
    Level &level;

    const float BULLET_WORLD_WIDTH = 16.0f;
    const int GRID_WIDTH;
    const int GRID_HEIGHT;
    const float DISTANCE_GRID_CELLS;
    const float DISTANCE_GRID_CELLS_INV;

    // NOTE: examples are using btAlignedObjectArray<btCollisionShape *>
    //       not sure if there's a performance penalty
    std::vector<std::unique_ptr<btCollisionShape>> collision_shapes;

    std::unique_ptr<btCollisionConfiguration> collision_configuration;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btBroadphaseInterface> broadphase;
    std::unique_ptr<btConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;
    std::unique_ptr<btCollisionShape> default_collision_shape;

    // TODO: int -> std::pair<unique_ptr(?)<RigidBody>, Transform>
    typedef std::pair<std::unique_ptr<btRigidBody>, std::unique_ptr<Transform>> RigidBodiesEntry;
    std::unordered_map<int, RigidBodiesEntry> rigid_bodies;
    std::unordered_map<int, glm::vec2> impulses;
    Array2D<Level::CellType> grid_static_objects_flow;
    Array2D<Level::CellType> grid_ball;
    Array2D<glm::vec2> grid_velocities;

    std::unique_ptr<btHingeConstraint> hinge_right;
    std::unique_ptr<btHingeConstraint> hinge_left;

public:
    RigidBodyPhysics(Level &level)
        : level(level), GRID_WIDTH(level.width), GRID_HEIGHT(level.height),
          DISTANCE_GRID_CELLS(BULLET_WORLD_WIDTH / GRID_WIDTH),
          DISTANCE_GRID_CELLS_INV(GRID_WIDTH / BULLET_WORLD_WIDTH),
          collision_configuration(std::make_unique<btDefaultCollisionConfiguration>()),
          dispatcher(std::make_unique<btCollisionDispatcher>(collision_configuration.get())),
          broadphase(std::make_unique<btDbvtBroadphase>()),
          solver(std::make_unique<btSequentialImpulseConstraintSolver>()),
          dynamics_world(std::make_unique<btDiscreteDynamicsWorld>(
              dispatcher.get(), broadphase.get(), solver.get(), collision_configuration.get())),
          default_collision_shape(std::make_unique<btSphereShape>(DISTANCE_GRID_CELLS)),
          grid_static_objects_flow(Array2D<Level::CellType>(GRID_WIDTH, GRID_HEIGHT)),
          grid_ball(Array2D<Level::CellType>(GRID_WIDTH, GRID_HEIGHT)),
          grid_velocities(Array2D<glm::vec2>(GRID_WIDTH, GRID_HEIGHT)) {
        dynamics_world->setGravity(btVector3(0.f, 0.f, 0.f));

        grid_static_objects_flow = level.matrix;

        for (const unique_ptr<RigidBody> &level_body : level.rigidBodies) {
            addRigidBody(*level_body);
        }

        addBoundaryRigidBodies();
    }

    bool isFlipper(int id);

    void addRigidBody(const RigidBody &level_body);

	// Add one rigid body that is invisible to user at an inflow cell
	void createBoundaryRigidBody(btCollisionShape *collision_shape,
		btDefaultMotionState *motion_state,	btRigidBody *bt_rigid_body,
		btTransform& transform, const int x, const int y, const int len, const int axis);

    // TODO: Instead of multiple rigid bodies just make a longer rectangle
    // Add rigid bodies invisible to the user at inflow cells
    void addBoundaryRigidBodies();

    std::unique_ptr<btRigidBody> createBtRigidBody(const RigidBody &level_body);

    // TODO: dtor (remove objects, etc. from world before removing from our owning containers)

    float gridToBullet(int x);
    glm::vec2 gridToBullet(int x, int y);
    float bulletToGrid(float x);
    glm::vec2 bulletToGrid(float x, float y);

    void clearDynamicFlagFields(Array2D<Level::CellType> &grid_obj);

    void markRBAsObstacles(Array2D<Level::CellType> &grid_obj);

    void markBallAsObstacle(int id, Array2D<glm::vec2> &grid_vel, btRigidBody *bt_rigid_body,
                            btVector3 &origin);

    void applyImpulses(btCollisionObject *&obj);

    // TODO give more meaningful name
    void processRigidBody(btCollisionObject *&obj, RigidBodyPhysicsOutput &output,
                          Array2D<glm::vec2> &grid_vel);

    void compute(const RigidBodyPhysicsInput &input, RigidBodyPhysicsOutput &output);

    void setGravity(bool on);

    Transform getRigidBody(int idx);

    void setRigidBody(int id, float x, float y, float mass, float rotation);

    void grid_finFlag(Array2D<Level::CellType> &grid_fin, Array2D<glm::vec2> &grid_vel,
                      btRigidBody *rigid_body);
};

#endif
