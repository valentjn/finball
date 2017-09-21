#ifndef RIGID_BODY_PHYSICS_HPP_
#define RIGID_BODY_PHYSICS_HPP_

#include <memory>

#include "btBulletDynamicsCommon.h"
#include "glm/vec2.hpp"

#include "Level.hpp"
#include "Parameters.hpp"
#include "RigidBodyPhysicsInput.hpp"
#include "RigidBodyPhysicsOutput.hpp"

class RigidBodyPhysics {
private:
    Parameters &parameters;
    Level &level;

    std::unique_ptr<btAlignedObjectArray<btCollisionShape *>> collision_shapes;

    std::unique_ptr<btCollisionConfiguration> collision_configuration;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btBroadphaseInterface> broadphase;
    std::unique_ptr<btConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;

public:
    RigidBodyPhysics(Parameters &parameters, Level &level)
        : parameters(parameters), level(level),
          collision_shapes(std::make_unique<btAlignedObjectArray<btCollisionShape *>>()),
          collision_configuration(std::make_unique<btDefaultCollisionConfiguration>()),
          dispatcher(std::make_unique<btCollisionDispatcher>(collision_configuration.get())),
          broadphase(std::make_unique<btDbvtBroadphase>()),
          solver(std::make_unique<btSequentialImpulseConstraintSolver>()),
          dynamics_world(std::make_unique<btDiscreteDynamicsWorld>(
              dispatcher.get(), broadphase.get(), solver.get(), collision_configuration.get())) {
        btCollisionShape *colShape = new btSphereShape(btScalar(1));

        collision_shapes->push_back(colShape);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(btScalar(8), btScalar(8), btScalar(0)));

        btScalar mass(1.f);

        btVector3 localInertia;
        colShape->calculateLocalInertia(mass, localInertia);

        btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);
        btRigidBody *body = new btRigidBody(mass, motionState, colShape, localInertia);
        dynamics_world->addRigidBody(body);

        // float grid_step_size = BULLET_WORLD_WIDTH / static_cast<float>(level.width);
    }

    // TODO: Destructor  colShapes

    void compute(const RigidBodyPhysicsInput &input, RigidBodyPhysicsOutput &output) {
        dynamics_world->stepSimulation(1. / 60.);
        auto &grid_obj = output.grid_objects;
        auto &grid_vel = output.grid_velocities;
        for (int i = 0; i < dynamics_world->getNumCollisionObjects(); i++) {
            auto &obj = dynamics_world->getCollisionObjectArray()[i];
            // TODO: determine which cells are occupied by obj

            btRigidBody *body = btRigidBody::upcast(obj);
            btTransform trans;
            if (body && body->getMotionState()) {
                body->getMotionState()->getWorldTransform(trans);
            } else {
                trans = obj->getWorldTransform();
            }
            btVector3 &origin = trans.getOrigin();
            printf("world pos object %d = %f,%f,%f\n", i, origin.getX(), origin.getY(),
                   origin.getZ());

            grid_obj->value(1, 3) = RigidBodyPhysicsOutput::type::DYNAMIC;
            grid_vel->value(1, 3) = glm::vec2{1.0, 0.5};
        }
    }
};

#endif
