#ifndef RIGID_BODY_PHYSICS_HPP_
#define RIGID_BODY_PHYSICS_HPP_

#include <btBulletDynamicsCommon.h>
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

    std::unordered_map<int, std::unique_ptr<Transform>> rigid_bodies;
    std::unordered_map<int, glm::vec2> impulses;
    Array2D<Level::CellType> grid_static_objects_flow;
    Array2D<Level::CellType> grid_ball;
    Array2D<Level::CellType> grid_fins;
    Array2D<glm::vec2> grid_velocities;

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
        dynamics_world->setGravity(btVector3(0.f, 0.f, 0.f));
        btCollisionShape *default_sphere_shape = new btSphereShape(DISTANCE_GRID_CELLS);

        grid_static_objects_flow = level.matrix;

        for (const unique_ptr<RigidBody> &level_body : level.rigidBodies) {
            initRigidBody(level_body, default_sphere_shape);
        }
    }

    void initRigidBody(const unique_ptr<RigidBody> &level_body, btCollisionShape * default_sphere_shape){
        btRigidBody* bt_rigid_body = createRigidBody(level_body, default_sphere_shape);

        // Constrain to two dimensions
        bt_rigid_body->setLinearFactor(btVector3(1.f, 1.f, 0.f));
        bt_rigid_body->setAngularFactor(btVector3(0.f, 0.f, 1.f));

        auto rigid_body =
                std::make_unique<Transform>(level_body->id, level_body->position, level_body->rotation);
        rigid_bodies[level_body->id] = std::move(rigid_body);

        // bt_rigid_body->setUserIndex2(static_cast<int>(detection_type));
        dynamics_world->addRigidBody(bt_rigid_body);

        if (level_body->mass == 0.f) {
            // TODO: do this more accurately?
            // TODO: this only works for rectangles atm
            grid_static_objects_flow.value(level_body->position.x, level_body->position.y) = Level::CellType::OBSTACLE;
        }
    }

    btRigidBody* createRigidBody(const unique_ptr<RigidBody> &level_body, btCollisionShape * default_sphere_shape){
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(level_body->position.x * DISTANCE_GRID_CELLS,
                                      level_body->position.y * DISTANCE_GRID_CELLS, 0.f));
        btDefaultMotionState *motion_state = new btDefaultMotionState(transform);
        btScalar mass = level_body->mass;
        btVector3 inertia;
        default_sphere_shape->calculateLocalInertia(mass, inertia);

        // TODO: set correct collision shape based on RigidBody type
        btRigidBody *bt_rigid_body;
        if (typeid(*level_body) == typeid(RigidBodyCircle)) {
            btCollisionShape *sphere_shape = new btSphereShape(static_cast<RigidBodyCircle *>(level_body.get())->radius * DISTANCE_GRID_CELLS); // FIXME: Memory leak
            bt_rigid_body = new btRigidBody(mass, motion_state, sphere_shape, inertia);
        } else {
            bt_rigid_body = new btRigidBody(mass, motion_state, default_sphere_shape, inertia);
        }
        bt_rigid_body->setUserIndex(level_body->id);
        return bt_rigid_body;
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
        output.rigid_bodies.clear();
        // TODO: try to give out a const reference to our internal rigid_bodies vector

        // Compute impulses
        // TODO: Make it work for multiple balls.
        input.computeImpulses(grid_ball, impulses);
        // TODO: apply impulses
        for (int j = 0; j < dynamics_world->getNumCollisionObjects(); j++) {
            auto &obj = dynamics_world->getCollisionObjectArray()[j];
            btRigidBody *bt_rigid_body = btRigidBody::upcast(obj);
            int id = bt_rigid_body->getUserIndex();
            Transform *rigid_body = rigid_bodies[id].get();
            if (bt_rigid_body && bt_rigid_body->getMotionState() && rigid_body->id == Level::BALL_ID) {
                bt_rigid_body->applyCentralImpulse(btVector3(impulses[1].x, impulses[1].y, 0.0f));
            }
        }

        // Clear the impulses for the next time step
        impulses.clear();

        // clear dynamic flag fields
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                grid_ball.value(x, y) = Level::CellType::FLUID;
                grid_obj.value(x, y) = grid_static_objects_flow.value(x, y);
                // grid_pedals.value(x, y) = false; // TODO
                grid_velocities.value(x, y) = glm::vec2{0., 0.};
            }
        }

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
                Transform *output_transform = rigid_bodies[id].get();

                // TODO: SCALE WITH DISTANCE_GRID_CELLS_INV
                output_transform->position.x = origin.getX() * DISTANCE_GRID_CELLS_INV;
                output_transform->position.y = origin.getY() * DISTANCE_GRID_CELLS_INV;
                // TODO: check that this behaves correctly
                auto quaternion = transform.getRotation();
                if (quaternion.getAxis().z() < 0.)
                {
                    output_transform->rotation = 2*M_PI-quaternion.getAngle();
                } else {
                    output_transform->rotation = quaternion.getAngle();
                }
                output.rigid_bodies.push_back(output_transform);

                if (id == Level::BALL_ID) {
                    for (int y = 0; y < GRID_HEIGHT; ++y) {
                        for (int x = 0; x < GRID_WIDTH; ++x) {
                            glm::vec2 pos = gridToBullet(x, y);
                            // printf("%f %f\n", pos.x, pos.y);
                            // printf("-------\n");
                            // float length = (pos - output_transform->position).length();
                            // printf("%f\n", length);
                            // TODO: scale with DISTANCE_GRID_CELLS
                            if (glm::distance(pos, glm::vec2{origin.getX(), origin.getY()}) <= static_cast<RigidBodyCircle *>(level.rigidBodies[id-1].get())->radius * DISTANCE_GRID_CELLS) {
                                grid_ball.value(x, y) = Level::CellType::OBSTACLE;
                                grid_vel.value(x, y) = glm::vec2(1.0, 2.0); // TODO
                            }
                        }
                    }
                }
            } else {
                Log::error("Problem in RigidBodyPhysics compute(): no RigidBody or could not get MotionState");
                throw std::runtime_error("Problem in RigidBodyPhysics compute(): no RigidBody or could not get MotionState");
            }
        }

        // TODO: mesh tohether the individual flag fields
        for (int y = GRID_HEIGHT - 1; y >= 0; --y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                // TODO: grid_pedals

                if (grid_ball.value(x, y) == Level::CellType::OBSTACLE) {
                    grid_obj.value(x, y) = Level::CellType::OBSTACLE;
                }
            }
        }

        // grid_finFlag(grid_obj, glm::vec2(0,0),
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

    void setGravity(bool on) {
        dynamics_world->setGravity(btVector3(0., (on ? -10. : 0.), 0.));
    }

    Transform getRigidBody(int idx){
        return *(rigid_bodies[idx].get());
    }

    void setRigidBody(int id, float x, float y, float mass = 1.f, float rotation = 0.f){
        //loop over the ones already there

        //create a new one
    }

    void grid_finFlag(Array2D<Level::CellType> &grid_fin, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3) {
        glm::vec2 norm1(-(pos1.y - pos2.y), pos1.x - pos2.x);
        glm::vec2 norm2(-(pos2.y - pos3.y), pos2.x - pos3.x);
        glm::vec2 norm3(-(pos3.y - pos1.y), pos3.x - pos1.x);

        for (int i = 0; i< grid_fin.width(); i++){
            for (int j= 0 ; j <grid_fin.height(); j++){
                glm::vec2 tempVec1 = gridToBullet(i, j) - pos1;
                glm::vec2 tempVec2 = gridToBullet(i, j) - pos2;
                glm::vec2 tempVec3 = gridToBullet(i, j) - pos3;
                if ((tempVec1.x * norm1.x + tempVec1.y * norm1.y >= 0) &&
                        (tempVec2.x * norm2.x + tempVec2.y * norm2.y >= 0) &&
                        (tempVec3.x * norm3.x + tempVec3.y * norm3.y >= 0)) {
                    grid_fin.value(i, j) = Level::CellType::OBSTACLE;
                }
            }
        }
    }
};

#endif
