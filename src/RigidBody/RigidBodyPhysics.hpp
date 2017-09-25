#ifndef RIGID_BODY_PHYSICS_HPP_
#define RIGID_BODY_PHYSICS_HPP_

#include <memory>
#include <unordered_map>

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "Level.hpp"
#include "Log.hpp"
#include "RigidBody/RigidBodyPhysicsInput.hpp"
#include "RigidBody/RigidBodyPhysicsOutput.hpp"

//TODO: Maybe name variables properly??
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

    std::unique_ptr<btAlignedObjectArray<btCollisionShape *>> collision_shapes;

    std::unique_ptr<btCollisionConfiguration> collision_configuration;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btBroadphaseInterface> broadphase;
    std::unique_ptr<btConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamics_world;

    std::unordered_map<int, std::unique_ptr<RigidBody>> rigid_bodies;
    Array2D<Level::CellType> grid_static_objects_flow;
    Array2D<Level::CellType> grid_ball;
    // Array2D<bool> grid_pedals;
    Array2D<glm::vec2> grid_velocities;

    int ball_id; // FIXME: ...
    float BALL_RADIUS; // FIXME: this should really not be necessary
    // FIXME: for now in LBM coords

    enum class DetectionType { CIRCLE, GENERAL };

public:
    RigidBodyPhysics(Level &level)
        : level(level), GRID_WIDTH(level.width), GRID_HEIGHT(level.height),
          DISTANCE_GRID_CELLS(BULLET_WORLD_WIDTH / GRID_WIDTH),
          DISTANCE_GRID_CELLS_INV(GRID_WIDTH / BULLET_WORLD_WIDTH), // TODO: currently end of grid != (BULLET_WORLD_WITH | BULLET_WORLD_WIDTH)
          collision_shapes(std::make_unique<btAlignedObjectArray<btCollisionShape *>>()),
          collision_configuration(std::make_unique<btDefaultCollisionConfiguration>()),
          dispatcher(std::make_unique<btCollisionDispatcher>(collision_configuration.get())),
          broadphase(std::make_unique<btDbvtBroadphase>()),
          solver(std::make_unique<btSequentialImpulseConstraintSolver>()),
          dynamics_world(std::make_unique<btDiscreteDynamicsWorld>(
              dispatcher.get(), broadphase.get(), solver.get(), collision_configuration.get())),
          grid_static_objects_flow(Array2D<Level::CellType>(GRID_WIDTH, GRID_HEIGHT)),
          grid_ball(Array2D<Level::CellType>(GRID_WIDTH, GRID_HEIGHT)),
          // grid_pedals(Array2D<Level::CellType>(GRID_WIDTH, GRID_HEIGHT)),
          grid_velocities(Array2D<glm::vec2>(GRID_WIDTH, GRID_HEIGHT))
	{
        // TODO: set correct radius once it's available; scale with DISTANCE_GRID_CELLS
        // TODO: static objects are rectangles for now
        BALL_RADIUS = DISTANCE_GRID_CELLS;
        btCollisionShape *sphere_shape = new btSphereShape(BALL_RADIUS);

        grid_static_objects_flow = level.matrix;

        // TODO: remove this
        static int next_id = 1;

        for (auto level_body : level.rigidBodies) {
            // DetectionType detection_type = DetectionType::GENERAL;
            if (!level_body.isFixed) {
                ball_id = next_id;
                // detection_type = DetectionType::CIRCLE;
            }

            btTransform transform;
            transform.setIdentity();
            transform.setOrigin(btVector3(level_body.position.x * DISTANCE_GRID_CELLS,
                level_body.position.y * DISTANCE_GRID_CELLS, 0));
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

            auto rigid_body = // TODO: fix id
                std::make_unique<RigidBody>(next_id, level_body.position.x, level_body.position.y);
            rigid_bodies[level_body.id] = std::move(rigid_body);

            bt_rigid_body->setUserIndex(level_body.id); // -> RigidBody.id
            // bt_rigid_body->setUserIndex2(static_cast<int>(detection_type));
            dynamics_world->addRigidBody(bt_rigid_body);

            next_id++;
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
        printf("%d\n", ball_id);
        auto &grid_obj = output.grid_objects;
        auto &grid_vel = output.grid_velocities;
        output.rigid_bodies.clear(); // currently not needed as we get a new output each time
        // TODO: try to give out a const reference to our internal rigid_bodies vector

        // clear dynamic flag fields
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                grid_ball.value(x, y) = Level::CellType::FLUID;
                // grid_pedals.value(x, y) = false; // TODO
                grid_velocities.value(x, y) = glm::vec2{0., 0.};
            }
        }

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
                rigid_body->position.x = origin.getX() * DISTANCE_GRID_CELLS_INV;
                rigid_body->position.y = origin.getY() * DISTANCE_GRID_CELLS_INV;
                // TODO: check that this behaves correctly
                rigid_body->angle = transform.getRotation().getAngle();
                output.rigid_bodies.push_back(rigid_body);

                // DetectionType detection_type =
                //     static_cast<DetectionType>(bt_rigid_body->getUserIndex2());
                // TODO: find other way than setUserIndex2 to store this
                // switch (detection_type) {
                // case DetectionType::CIRCLE:
                if (rigid_body->id == ball_id) {
                    for (int y = 0; y < GRID_HEIGHT; ++y) {
                        for (int x = 0; x < GRID_WIDTH; ++x) {
                            glm::vec2 pos = gridToBullet(x, y);
                            // printf("%f %f\n", pos.x, pos.y);
                            // printf("-------\n");
                            // float length = (pos - rigid_body->position).length();
                            // printf("%f\n", length);
                            if (glm::distance(pos, glm::vec2{origin.getX(), origin.getY()}) <= BALL_RADIUS) {
                                // printf("ASDSADAd\n");
                                grid_ball.value(x, y) = Level::CellType::OBSTACLE;
                                grid_vel.value(x, y) = glm::vec2(1.0, 2.0); // TODO
                            }
                        }
                    }
                }
                    // break;
                // default:
                    // Log::error("Unimplemented detection type!");
                // }
            } else {
                assert(false);
            }
        }

        // TODO: mesh tohether the individual flag fleids
        for (int y = GRID_HEIGHT - 1; y >= 0; --y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                // TODO: grid_pedals
                if (grid_static_objects_flow.value(x, y) == Level::CellType::INFLOW  ) {
                    grid_obj.value(x, y) = Level::CellType::INFLOW;
                } else if (grid_static_objects_flow.value(x, y) == Level::CellType::OUTFLOW  ) {
                    grid_obj.value(x, y) = Level::CellType::OUTFLOW;
                } else if (grid_static_objects_flow.value(x, y) == Level::CellType::OBSTACLE) {
                    grid_obj.value(x, y) = Level::CellType::OBSTACLE;
                }

                if (grid_ball.value(x, y) == Level::CellType::OBSTACLE) {
                    grid_obj.value(x, y) = Level::CellType::OBSTACLE;
                }
            }
        }

        // for (int i = GRID_HEIGHT - 1; i >= 0; --i) {
        //     for (int j = 0; j < GRID_WIDTH; ++j) {
        //         printf("%2d", grid_ball.value(j, i));
        //     }
        //     printf("\n");
        // }
        // printf("-----------\n");

        // TODO: determine which cells are occupied by obj
        // grid_obj.value(1, 3) = Level::CellType::FLUID;
        // grid_vel.value(1, 3) = glm::vec2{1.0, 0.5};
    }

    void setGravity(bool on){
        if(on)
            dynamics_world->setGravity(btVector3(0.,-10.,0.));
        else
            dynamics_world->setGravity(btVector3(0.,0.,0.));
    }
};

#endif
