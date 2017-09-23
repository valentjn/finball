#ifndef RIGID_BODY_PHYSICS_HPP_
#define RIGID_BODY_PHYSICS_HPP_

#include <memory>

#include "btBulletDynamicsCommon.h"
#include "glm/vec2.hpp"

#include "Level.hpp"
#include "RigidBodyPhysicsInput.hpp"
#include "RigidBodyPhysicsOutput.hpp"

class RigidBodyPhysics {
private:
    Level &level;

    std::unique_ptr<btAlignedObjectArray<btCollisionShape *>> collision_shapes;

    std::unique_ptr<btCollisionConfiguration> collision_configuration;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btBroadphaseInterface> broadphase;
    std::unique_ptr<btConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;

public:
    RigidBodyPhysics(Level &level)
        : level(level),
          collision_shapes(std::make_unique<btAlignedObjectArray<btCollisionShape *>>()),
          collision_configuration(std::make_unique<btDefaultCollisionConfiguration>()),
          dispatcher(std::make_unique<btCollisionDispatcher>(collision_configuration.get())),
          broadphase(std::make_unique<btDbvtBroadphase>()),
          solver(std::make_unique<btSequentialImpulseConstraintSolver>()),
          dynamics_world(std::make_unique<btDiscreteDynamicsWorld>(
              dispatcher.get(), broadphase.get(), solver.get(), collision_configuration.get())) {
        btCollisionShape *sphere_shape =
            new btSphereShape(1); // TODO: set correct radius once it's available
        for (auto rigidBody : level.rigidBodies) {
            btTransform transform;
            transform.setIdentity();
            transform.setOrigin(btVector3(rigidBody.position.x, rigidBody.position.y, 0));
            btDefaultMotionState *motion_state = new btDefaultMotionState(transform);
            btScalar mass = 1;
            if (rigidBody.isFixed) {
                mass = 0;
            }
            btVector3 inertia;
            sphere_shape->calculateLocalInertia(mass, inertia);
            btRigidBody *rigid_body = new btRigidBody(mass, motion_state, sphere_shape, inertia);
            dynamics_world->addRigidBody(rigid_body);
        }
    }

    void compute(const RigidBodyPhysicsInput &input, RigidBodyPhysicsOutput &output) {
        dynamics_world->stepSimulation(1. / 60.); // TODO: everybody has to use the same timestep

        auto &grid_obj = output.grid_objects;
        auto &grid_vel = output.grid_velocities;
        output.rigid_bodies.clear();
        for (int j = 0; j < dynamics_world->getNumCollisionObjects(); j++) {
            auto &obj = dynamics_world->getCollisionObjectArray()[j];
            btRigidBody *rigid_body = btRigidBody::upcast(obj);
            btTransform trans;
            if (rigid_body && rigid_body->getMotionState()) {
                rigid_body->getMotionState()->getWorldTransform(trans);
            } else {
                trans = obj->getWorldTransform();
            }
            btVector3 &origin = trans.getOrigin();
            RigidBody output_body(origin.getX(), origin.getY());
            output.rigid_bodies.push_back(output_body);

            // TODO: determine which cells are occupied by obj
            grid_obj.value(1, 3) = Level::CellType::FLUID;
            grid_vel.value(1, 3) = glm::vec2{1.0, 0.5};
        }
    }
};

#endif
