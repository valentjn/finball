#include "FaExample.h"

#include "LinearMath/btAlignedObjectArray.h"
#include "LinearMath/btVector3.h"
#include "btBulletDynamicsCommon.h"

#include "../CommonInterfaces/CommonRigidBodyBase.h"

#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_Z 5

struct FaExample : public CommonRigidBodyBase {
    FaExample(struct GUIHelperInterface *helper)
        : CommonRigidBodyBase(helper) {}

    void initPhysics() {
        m_collisionConfiguration = new btDefaultCollisionConfiguration();
        m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
        m_broadphase = new btDbvtBroadphase();
        m_solver = new btSequentialImpulseConstraintSolver;
        m_dynamicsWorld = new btDiscreteDynamicsWorld(
            m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);

        btCollisionShape *groundShape =
            new btStaticPlaneShape(btVector3(0, 1, 0), 0);

        btTransform groundTransform;
        groundTransform.setIdentity();
        btDefaultMotionState *groundMotionState =
            new btDefaultMotionState(groundTransform);

        btScalar groundMass(0.); // -> static
        btRigidBody *body =
            new btRigidBody(groundMass, groundMotionState, groundShape);
        body->setFriction(1);

        m_dynamicsWorld->addRigidBody(body);

        btCollisionShape *colShape = new btBoxShape(btVector3(5, 5, 5));
        // btCollisionShape *colShape = new btSphereShape(btScalar(0.15));

        m_collisionShapes.push_back(colShape);

        btTransform startTransform;
        startTransform.setIdentity();

        btScalar mass(1.f);

        btVector3 localInertia;
        colShape->calculateLocalInertia(mass, localInertia);

        for (int k = 0; k < ARRAY_SIZE_Y; k++) {
            for (int i = 0; i < ARRAY_SIZE_X; i++) {
                for (int j = 0; j < ARRAY_SIZE_Z; j++) {
                    startTransform.setOrigin(btVector3(btScalar(0.2 * i),
                                                       btScalar(2 + 0.2 * k),
                                                       btScalar(0.2 * j)));

                    btDefaultMotionState *motionState =
                        new btDefaultMotionState(startTransform);
                    btRigidBody *body = new btRigidBody(mass, motionState,
                                                        colShape, localInertia);
                    m_dynamicsWorld->addRigidBody(body);
                }
            }
        }

        m_guiHelper->autogenerateGraphicsObjects(m_dynamicsWorld);
    }

    void resetCamera() {
        float dist = 4;
        float pitch = -35;
        float yaw = 52;
        float targetPos[3] = {0, 0, 0};
        m_guiHelper->resetCamera(dist, yaw, pitch, targetPos[0], targetPos[1],
                                 targetPos[2]);
    }
};

CommonExampleInterface *FaExampleCreateFunc(CommonExampleOptions &options) {
    return new FaExample(options.m_guiHelper);
}

B3_STANDALONE_EXAMPLE(FaExampleCreateFunc)
