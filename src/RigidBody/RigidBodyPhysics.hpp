#ifndef RIGID_BODY_PHYSICS_HPP_
#define RIGID_BODY_PHYSICS_HPP_

#include <memory>
#include <unordered_map>

#include <btBulletDynamicsCommon.h>
#include <glm/vec2.hpp>

#include "Level.hpp"
#include "RigidBody/RigidBodyPhysicsInput.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"

// GET THE BODY SHAPE RESPONSIBILITY THING SORTED OUT!!!! (global id for each object)
// then only send out transform related to the objects with that id (rename RigidBody to Transform)
// (convince others to use r_ball = GRID_WIDTH / 16 / 2)
// use applyCentralImpulse (ignore torque)

// (try to disallow id == 0 so we can thn)
// (0 0 0 00)
// (0 0-1 00)
// (0-1 1-10)
// (0 0-1 00)
// (00000000)
// where 1 is the body and -1 the surrounding cells (only for dynamic objects (ball; pedals)) -> impulse is applied there (where? negative field?; positive field? what if there are two???)
// (for static objs we don't beed the adjacent cells as no force applies on them anyway)

// different set of flagfields for static objs, ball, ...? (ball is just clear everything and mark
// cells where dist <= radius; static just stays the same (id in the field for easy removal; needs
// to be converted to CellType))
// pedals will need either ghost objects or triangle rendering (depends on exact shape)
// ghost objects only in that bottom area?

class RigidBodyPhysics {
private:
    Level &level;

    std::unique_ptr<btAlignedObjectArray<btCollisionShape *>> collision_shapes;

    std::unique_ptr<btCollisionConfiguration> collision_configuration;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btBroadphaseInterface> broadphase;
    std::unique_ptr<btConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;

    std::unordered_map<int, std::unique_ptr<RigidBody>> rigid_bodies;

    const float BULLET_WORLD_WIDTH = 16.0f;
    const int GRID_WIDTH;
    const int GRID_HEIGHT;
    const float DISTANCE_GRID_CELLS;
    const float DISTANCE_GRID_CELLS_INV;

public:
    RigidBodyPhysics(Level &level)
        : level(level),
          collision_shapes(std::make_unique<btAlignedObjectArray<btCollisionShape *>>()),
          collision_configuration(std::make_unique<btDefaultCollisionConfiguration>()),
          dispatcher(std::make_unique<btCollisionDispatcher>(collision_configuration.get())),
          broadphase(std::make_unique<btDbvtBroadphase>()),
          solver(std::make_unique<btSequentialImpulseConstraintSolver>()),
          dynamics_world(std::make_unique<btDiscreteDynamicsWorld>(
              dispatcher.get(), broadphase.get(), solver.get(), collision_configuration.get())),
          GRID_WIDTH(level.width), GRID_HEIGHT(level.height),
          DISTANCE_GRID_CELLS(BULLET_WORLD_WIDTH / GRID_WIDTH),
          DISTANCE_GRID_CELLS_INV(1 / DISTANCE_GRID_CELLS) {
        btCollisionShape *sphere_shape = new btSphereShape(
            1); // TODO: set correct radius once it's available; scale with DISTANCE_GRID_CELLS
        for (auto level_body : level.rigidBodies) {
            btTransform transform;
            transform.setIdentity();
            // TODO: scale with DISTANCE_GRID_CELLS
            transform.setOrigin(btVector3(level_body.position.x, level_body.position.y, 0));
            btDefaultMotionState *motion_state = new btDefaultMotionState(transform);
            btScalar mass = 1;
            if (level_body.isFixed) {
                mass = 0;
            }
            btVector3 inertia;
            sphere_shape->calculateLocalInertia(mass, inertia);
            btRigidBody *bt_rigid_body = new btRigidBody(mass, motion_state, sphere_shape, inertia);

            // Constrain to two dimensions
            bt_rigid_body->setLinearFactor(btVector3(1, 1, 0));
            bt_rigid_body->setAngularFactor(btVector3(0, 0, 1));

            // TODO: remove this
            auto rigid_body =
                std::make_unique<RigidBody>(level_body.id, level_body.position.x, level_body.position.y);
            rigid_bodies[level_body.id] = std::move(rigid_body);

            bt_rigid_body->setUserIndex(level_body.id); // -> RigidBody.id

            dynamics_world->addRigidBody(bt_rigid_body);
        }
    }

    // TODO: dtor

    glm::vec2 gridToBullet(int x, int y) {
        return glm::vec2(static_cast<float>(x) * DISTANCE_GRID_CELLS,
                         static_cast<float>(y) * DISTANCE_GRID_CELLS);
    }

    glm::vec2 bulletToGrid(float x, float y) {
        return glm::vec2(x * DISTANCE_GRID_CELLS_INV, y * DISTANCE_GRID_CELLS_INV);
    }

    void compute(const RigidBodyPhysicsInput &input, RigidBodyPhysicsOutput &output) {
        auto &grid_obj = output.grid_objects;
        auto &grid_vel = output.grid_velocities;
        output.rigid_bodies.clear(); // currently not needed as we get a new output each time
        // TODO: try to give out a const reference to our internal rigid_bodies vector

        // TODO: apply impulses

        dynamics_world->stepSimulation(1. / 60.); // TODO: everybody has to use the same timestep

        for (int j = 0; j < dynamics_world->getNumCollisionObjects(); j++) {
            auto &obj = dynamics_world->getCollisionObjectArray()[j];
            btRigidBody *bt_rigid_body = btRigidBody::upcast(obj);
            btTransform transform;
            if (bt_rigid_body && bt_rigid_body->getMotionState()) {
                bt_rigid_body->getMotionState()->getWorldTransform(transform);
                btVector3 &origin = transform.getOrigin();

                // update the RigidBody and push the reference to our output
                int id = bt_rigid_body->getUserIndex();
                RigidBody *rigid_body = rigid_bodies[id].get();

                // TODO: SCALE WITH DISTANCE_GRID_CELLS_INV
                rigid_body->position.x = origin.getX();
                rigid_body->position.y = origin.getY();
                // TODO: check that this behaves correctly
                rigid_body->angle = transform.getRotation().getAngle();
                output.rigid_bodies.push_back(rigid_body);
            } else {
                assert(false);
            }
        }

        // TODO: determine which cells are occupied by obj
        grid_obj.value(1, 3) = Level::CellType::FLUID;
        grid_vel.value(1, 3) = glm::vec2{1.0, 0.5};
    }
};

#endif
