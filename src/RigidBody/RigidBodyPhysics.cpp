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

#include "RigidBodyPhysics.hpp"

void RigidBodyPhysics::addRigidBody(const RigidBody &level_body) {
    std::unique_ptr<btRigidBody> bt_rigid_body = createBtRigidBody(level_body);
    dynamics_world->addRigidBody(bt_rigid_body.get());
	bt_rigid_body->setRestitution(1.0f);
    if (isFlipper(level_body.id)) {
        btVector3 axis;
        if (level_body.id == level.flipperLeftId) {
            axis = btVector3(0, 0, 1);
        } else {
            axis = btVector3(0, 0, -1);
        }

        bt_rigid_body->setActivationState(DISABLE_DEACTIVATION);
        auto hinge = std::make_unique<btHingeConstraint>(*bt_rigid_body, btVector3(0, 0, 0), axis);
        hinge->setLimit(0, SIMD_PI / 2);
        hinge->setMaxMotorImpulse(SIMD_INFINITY); // FIXME?: infinity may be problematic
        dynamics_world->addConstraint(hinge.get());

        if (level_body.id == level.flipperLeftId) {
            hinge_left = std::move(hinge);
        } else {
            hinge_right = std::move(hinge);
        }
    } else {
        bt_rigid_body->setLinearFactor(btVector3(1.f, 1.f, 0.f));
        bt_rigid_body->setAngularFactor(btVector3(0.f, 0.f, 1.f));
    }

	if (level_body.id == level.BALL_ID) {
		bt_rigid_body->setRestitution(0.8f);
	}

    if (level_body.mass == 0.f) {
        // FIXME: not every objects is a rectangle...
        // FIXME: take width and height into consideration
        grid_static_objects_flow.value(level_body.position.x, level_body.position.y) =
            Level::CellType::OBSTACLE;
    }

    auto transform =
        std::make_unique<Transform>(level_body.id, level_body.position, level_body.rotation);
    rigid_bodies[level_body.id] =
        std::make_pair<std::unique_ptr<btRigidBody>, std::unique_ptr<Transform>>(
            std::move(bt_rigid_body), std::move(transform));
}

// Add one rigid body that is invisible to user at an inflow cell
void RigidBodyPhysics::createBoundaryRigidBody(btCollisionShape *collision_shape,
                                               btDefaultMotionState *motion_state,
                                               btRigidBody *bt_rigid_body, btTransform &transform,
                                               const int x, const int y, const int len,
                                               const int axis) {
    float xExtent = 0.5f, yExtent = 0.5f;
    float xOrig = x, yOrig = y;
    if (axis == 0) {
        xExtent = len / 2.0;
        xOrig -= xExtent;
    } else {
        yExtent = len / 2.0;
        yOrig -= yExtent;
    }
    btVector3 extents = DISTANCE_GRID_CELLS * btVector3(xExtent, yExtent, 0.0f); // One grid cell
    collision_shape = new btBoxShape(extents);
    transform.setOrigin(DISTANCE_GRID_CELLS * btVector3(xOrig, yOrig, 0.0f));
    motion_state = new btDefaultMotionState(transform);
    bt_rigid_body =
        new btRigidBody(0.0f, motion_state, collision_shape, btVector3(0.0f, 0.0f, 0.0f));
    bt_rigid_body->setUserIndex(-1); // Set user index to -1 to distinguish from obstacles
	bt_rigid_body->setRestitution(1.0f);
    dynamics_world->addRigidBody(bt_rigid_body);
}

// Add rigid bodies invisible to the user at inflow cells
void RigidBodyPhysics::addBoundaryRigidBodies() {
    btCollisionShape *collision_shape;
    btDefaultMotionState *motion_state;
    btRigidBody *bt_rigid_body;
    btTransform transform;
    transform.setIdentity();
    int x = 0, y = 0;
    // Use two for loops as we only want to iterate over the wall
    int len1 = 0, len2 = 0;
    // Left and right walls
    for (y = 0; y < grid_static_objects_flow.height(); ++y) {
        // Left wall
        x = 0;
        if (grid_static_objects_flow.value(x, y) == Level::CellType::INFLOW) {
            len1++;
        } else if (len1 > 0) {
            createBoundaryRigidBody(collision_shape, motion_state, bt_rigid_body, transform, x, y,
                                    len1, 1);
            len1 = 0;
        }
        // Right wall
        x = grid_static_objects_flow.width() - 1;
        if (grid_static_objects_flow.value(x, y) == Level::CellType::INFLOW) {
            len2++;
        } else if (len2 > 0) {
            createBoundaryRigidBody(collision_shape, motion_state, bt_rigid_body, transform, x, y,
                                    len2, 1);
            len2 = 0;
        }
    }
    // Take care of inflow that ends at the last cell
    if (len1 > 0) {
        createBoundaryRigidBody(collision_shape, motion_state, bt_rigid_body, transform, x, y, len1,
                                1);
    }
    if (len2 > 0) {
        createBoundaryRigidBody(collision_shape, motion_state, bt_rigid_body, transform, x, y, len2,
                                1);
    }
    len1 = 0;
    len2 = 0;
    // Top and bottom
    for (x = 0; x < grid_static_objects_flow.width(); ++x) {
        // Bottom wall
        y = 0;
        if (grid_static_objects_flow.value(x, y) == Level::CellType::INFLOW) {
            len1++;
        } else if (len1 > 0) {
            createBoundaryRigidBody(collision_shape, motion_state, bt_rigid_body, transform, x, y,
                                    len1, 0);
            len1 = 0;
        }
        // Top wall
        y = grid_static_objects_flow.height() - 1;
        if (grid_static_objects_flow.value(x, y) == Level::CellType::INFLOW) {
            len2++;
        } else if (len2 > 0) {
            createBoundaryRigidBody(collision_shape, motion_state, bt_rigid_body, transform, x, y,
                                    len2, 0);
            len2 = 0;
        }
    }
    // Take care of inflow that ends at the last cell
    if (len1 > 0) {
        createBoundaryRigidBody(collision_shape, motion_state, bt_rigid_body, transform, x, y, len1,
                                1);
    }
    if (len2 > 0) {
        createBoundaryRigidBody(collision_shape, motion_state, bt_rigid_body, transform, x, y, len2,
                                1);
    }
}

bool RigidBodyPhysics::isFlipper(int id) {
    return id == level.flipperLeftId || id == level.flipperRightId;
}

std::unique_ptr<btRigidBody> RigidBodyPhysics::createBtRigidBody(const RigidBody &level_body) {
    std::unique_ptr<btCollisionShape> collision_shape;
    if (const auto *circle = dynamic_cast<const RigidBodyCircle *>(&level_body)) {
        Log::info("Creating sphere collision shape.");
        // FIXME: Memory leak
        collision_shape = std::make_unique<btSphereShape>(circle->radius * DISTANCE_GRID_CELLS);
    } else if (const auto *rectangle = dynamic_cast<const RigidBodyRect *>(&level_body)) {
        Log::info("Creating rectangle collision shape.");
        // FIXME: Memory leak
        collision_shape = std::make_unique<btBoxShape>(
            btVector3(rectangle->width / 2., rectangle->height / 2., 0) *
            DISTANCE_GRID_CELLS); // these are half-extents!
    } else if (const auto *triangle = dynamic_cast<const RigidBodyTriangle *>(&level_body)) {
        Log::info("Creating triangle collision shape.");
        btVector3 p0 =
            btVector3(triangle->points[0].x, triangle->points[0].y, 0.) * DISTANCE_GRID_CELLS;
        btVector3 p1 =
            btVector3(triangle->points[1].x, triangle->points[1].y, 0.) * DISTANCE_GRID_CELLS;
        btVector3 p2 =
            btVector3(triangle->points[2].x, triangle->points[2].y, 0.) * DISTANCE_GRID_CELLS;

        auto convex_hull = std::make_unique<btConvexHullShape>();
        convex_hull->addPoint(p0);
        convex_hull->addPoint(p1);
        convex_hull->addPoint(p2);
        collision_shape = std::move(convex_hull);
    }

    auto *collision_shape_ptr = collision_shape.get();
    if (!collision_shape_ptr) {
        Log::error("RigidBody didn't have a specific shape! Creating a default sphere.");
        collision_shape_ptr = default_collision_shape.get();
    }
    collision_shapes.push_back(std::move(collision_shape));

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(level_body.position.x * DISTANCE_GRID_CELLS,
                                  level_body.position.y * DISTANCE_GRID_CELLS, 0.f));
    btDefaultMotionState *motion_state = new btDefaultMotionState(transform);

    btVector3 inertia;
    collision_shape_ptr->calculateLocalInertia(level_body.mass, inertia);

    auto bt_rigid_body =
        std::make_unique<btRigidBody>(level_body.mass, motion_state, collision_shape_ptr, inertia);
    bt_rigid_body->setUserIndex(level_body.id);
    return std::move(bt_rigid_body);
}

// TODO: dtor

float RigidBodyPhysics::gridToBullet(int x) { return static_cast<float>(x) * DISTANCE_GRID_CELLS; }

glm::vec2 RigidBodyPhysics::gridToBullet(int x, int y) {
    return glm::vec2(gridToBullet(x), gridToBullet(y));
}

float RigidBodyPhysics::bulletToGrid(float x) { return x * DISTANCE_GRID_CELLS_INV; }

glm::vec2 RigidBodyPhysics::bulletToGrid(float x, float y) {
    return glm::vec2(bulletToGrid(x), bulletToGrid(y));
}

void RigidBodyPhysics::clearDynamicFlagFields(Array2D<Level::CellType> &grid_obj) {
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            grid_ball.value(x, y) = Level::CellType::FLUID;
            grid_obj.value(x, y) = grid_static_objects_flow.value(x, y);
            // grid_pedals.value(x, y) = false; // TODO
            grid_velocities.value(x, y) = glm::vec2{0., 0.};
        }
    }
}

void RigidBodyPhysics::markRBAsObstacles(Array2D<Level::CellType> &grid_obj) {
    for (int y = GRID_HEIGHT - 1; y >= 0; --y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            // TODO: grid_pedals

            if (grid_ball.value(x, y) == Level::CellType::OBSTACLE) {
                grid_obj.value(x, y) = Level::CellType::OBSTACLE;
            }
        }
    }
}

void RigidBodyPhysics::markBallAsObstacle(int id, Array2D<glm::vec2> &grid_vel,
                                          btRigidBody *bt_rigid_body, btVector3 &origin) {
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            glm::vec2 pos = gridToBullet(x, y);
            if (glm::distance(pos, glm::vec2{origin.getX(), origin.getY()}) <=
                gridToBullet(
                    static_cast<RigidBodyCircle *>(level.rigidBodies[id - 1].get())->radius)) {
                grid_ball.value(x, y) = Level::CellType::OBSTACLE;
                // TODO: Proper scaling of velocity. Right now only scale the length.
                grid_vel.value(x, y) = gridToBullet(bt_rigid_body->getLinearVelocity().x(),
                                                    bt_rigid_body->getLinearVelocity().y());
            }
        }
    }
}

void RigidBodyPhysics::applyImpulses(btCollisionObject *&obj) {
    btRigidBody *bt_rigid_body = btRigidBody::upcast(obj);
    int id = bt_rigid_body->getUserIndex();
    Transform *rigid_body = rigid_bodies[id].second.get();
    if (bt_rigid_body && bt_rigid_body->getMotionState() && rigid_body->id == Level::BALL_ID) {
        bt_rigid_body->applyCentralImpulse(DISTANCE_GRID_CELLS *
                                           btVector3(impulses[1].x, impulses[1].y, 0.0f));
    }
}

// TODO give more meaningful name
void RigidBodyPhysics::processRigidBody(btCollisionObject *&obj, RigidBodyPhysicsOutput &output,
                                        Array2D<glm::vec2> &grid_vel) {
    btRigidBody *bt_rigid_body = btRigidBody::upcast(obj);
    btTransform transform;
    if (bt_rigid_body && bt_rigid_body->getMotionState()) {
        bt_rigid_body->getMotionState()->getWorldTransform(transform);
        btVector3 &origin = transform.getOrigin();

        // update the RigidBody and push the reference to our output
        int id = bt_rigid_body->getUserIndex();
        Transform *output_transform = rigid_bodies[id].second.get();

        output_transform->position.x = bulletToGrid(origin.getX());
        output_transform->position.y = bulletToGrid(origin.getY());

        // TODO: check that this behaves correctly
        auto quaternion = transform.getRotation();
        if (quaternion.getAxis().z() < 0.) {
            output_transform->rotation = 2 * SIMD_PI - quaternion.getAngle();
        } else {
            output_transform->rotation = quaternion.getAngle();
        }

        output.rigid_bodies.push_back(output_transform);

        if (id == Level::BALL_ID) {
            markBallAsObstacle(id, grid_vel, bt_rigid_body, origin);
        }
    } else {
        Log::error(
            "Problem in RigidBodyPhysics compute(): no RigidBody or could not get MotionState");
        throw std::runtime_error(
            "Problem in RigidBodyPhysics compute(): no RigidBody or could not get MotionState");
    }
}

void RigidBodyPhysics::compute(const RigidBodyPhysicsInput &input, RigidBodyPhysicsOutput &output) {
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

    float delta_angle_left = abs(hinge_left->getHingeAngle() - input.leftAngle);
    float delta_angle_right = abs(hinge_left->getHingeAngle() - input.rightAngle);
    hinge_left->setMotorTarget(input.leftAngle, 1.);
    hinge_right->setMotorTarget(input.rightAngle, 1.);
    if (delta_angle_left < SIMD_PI / 72) { // TODO: constexpr
        hinge_left->enableMotor(true);
    }
    if (delta_angle_right < SIMD_PI / 72) { // TODO: constexpr
        hinge_right->enableMotor(true);
    }
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
        if (isFlipper(rigid_body->getUserIndex())) {
            if (!rigid_body) {
                throw std::runtime_error("Fin is not a btRigidBody!");
            }
            grid_finFlag(grid_obj, grid_vel, rigid_body);
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

void RigidBodyPhysics::setGravity(bool on) {
    dynamics_world->setGravity(btVector3(0., (on ? -10. : 0.), 0.));
}

// TODO: this returns a transform...
Transform RigidBodyPhysics::getRigidBody(int idx) { return *(rigid_bodies[idx].second.get()); }

void RigidBodyPhysics::setRigidBody(int id, float x, float y, float mass = 1.f,
                                    float rotation = 0.f) {
    // loop over the ones already there

    // create a new one
}

void RigidBodyPhysics::grid_finFlag(Array2D<Level::CellType> &grid_fin,
                                    Array2D<glm::vec2> &grid_vel, btRigidBody *rigid_body) {
    btTransform transform;
    rigid_body->getMotionState()->getWorldTransform(transform);
    // TODO: use typeid to check that the shape is correct? (col_shape.getShapeType())
    // const btTriangleShapeEx *triangle_shape = static_cast<const btTriangleShapeEx
    // *>(rigid_body->getCollisionShape());
    const btConvexHullShape *triangle_shape =
        static_cast<const btConvexHullShape *>(rigid_body->getCollisionShape());

    const btVector3 &p1 = triangle_shape->getUnscaledPoints()[0];
    const btVector3 &p2 = triangle_shape->getUnscaledPoints()[1];
    const btVector3 &p3 = triangle_shape->getUnscaledPoints()[2];

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

    btVector3 localVel(0, 0, 0);
    glm::vec2 g2b = gridToBullet(0, 0);
    // TODO: only check in the AABB
    for (int i = 0; i < grid_fin.width(); i++) {
        for (int j = 0; j < grid_fin.height(); j++) {
            g2b = gridToBullet(i, j);
            glm::vec2 tempVec1 = g2b - pos1;
            glm::vec2 tempVec2 = g2b - pos2;
            glm::vec2 tempVec3 = g2b - pos3;
            if ((tempVec1.x * norm1.x + tempVec1.y * norm1.y >= 0) &&
                (tempVec2.x * norm2.x + tempVec2.y * norm2.y >= 0) &&
                (tempVec3.x * norm3.x + tempVec3.y * norm3.y >= 0)) {
                grid_fin.value(i, j) = Level::CellType::OBSTACLE;
                localVel =
                    rigid_body->getVelocityInLocalPoint(transform * btVector3(g2b.x, g2b.y, 0.0f));
                localVel *= DISTANCE_GRID_CELLS; // TODO: Scaling???
                grid_vel.value(i, j) = {localVel.x(), localVel.y()};
            }
        }
    }
}
