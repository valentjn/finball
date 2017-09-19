#ifndef RIGID_BODY_HPP_
#define RIGID_BODY_HPP_

#include <memory>

#include "btBulletDynamicsCommon.h"
#include "glm/vec2.hpp"

#include "Parameters.hpp"
#include "Level.hpp"
#include "RigidBodyInput.hpp"
#include "RigidBodyOutput.hpp"

class RigidBody {
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
    RigidBody(Parameters &parameters, Level &level) :
        parameters(parameters),
        level(level),
        collision_shapes(std::make_unique<btAlignedObjectArray<btCollisionShape *>>()),
        collision_configuration(std::make_unique<btDefaultCollisionConfiguration>()),
        dispatcher(std::make_unique<btCollisionDispatcher>(collision_configuration.get())),
        broadphase(std::make_unique<btDbvtBroadphase>()),
        solver(std::make_unique<btSequentialImpulseConstraintSolver>()),
        dynamics_world(std::make_unique<btDiscreteDynamicsWorld>(
            dispatcher.get(), broadphase.get(), solver.get(), collision_configuration.get()))
    {}

    void compute(const RigidBodyInput &input,
                 RigidBodyOutput &rigidBodyOutput)
    {
        auto &grid_obj = rigidBodyOutput.grid_objects;
        auto &grid_vel = rigidBodyOutput.grid_velocities;
        for (int j = 0; j < dynamics_world->getNumCollisionObjects(); j++) {
            auto &obj = dynamics_world->getCollisionObjectArray()[j];
            // TODO: determine which cells are occupied by obj
            grid_obj->setValue(1, 3, RigidBodyOutput::type::DYNAMIC);
            grid_vel->setValue(1, 3, glm::vec2 {1.0, 0.5});
        }

    }
};

#endif
