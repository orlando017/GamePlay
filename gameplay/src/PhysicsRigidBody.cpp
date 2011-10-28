/*
 * PhysicsRigidBody.cpp
 */

#include "Base.h"
#include "Game.h"
#include "PhysicsController.h"
#include "PhysicsMotionState.h"
#include "PhysicsRigidBody.h"

namespace gameplay
{

PhysicsRigidBody::PhysicsRigidBody(Node* node, PhysicsRigidBody::Type type, float mass, 
		float friction, float restitution, float linearDamping, float angularDamping)
        : _node(node), _body(NULL)
{
	switch (type)
	{
		case PhysicsRigidBody::PHYSICS_SHAPE_BOX:
		{
			const BoundingBox& box = node->getModel()->getMesh()->getBoundingBox();

            PhysicsController* physics = Game::getInstance()->getPhysicsController();
            btCollisionShape* shape = physics->getBox(box.min, box.max, btVector3(node->getScaleX(), node->getScaleY(), node->getScaleZ()));
			
			_body = createBulletRigidBody(shape, mass, node, friction, restitution, linearDamping, angularDamping);
			break;
		}
		case PhysicsRigidBody::PHYSICS_SHAPE_SPHERE:
		{
			const BoundingSphere& sphere = node->getModel()->getMesh()->getBoundingSphere();

			PhysicsController* physics = Game::getInstance()->getPhysicsController();
			btCollisionShape* shape = physics->getSphere(sphere.radius, btVector3(node->getScaleX(), node->getScaleY(), node->getScaleZ()));

            _body = createBulletRigidBody(shape, mass, node, friction, restitution, linearDamping, angularDamping);
			break;
		}
		case PhysicsRigidBody::PHYSICS_SHAPE_TRIANGLE_MESH:
		{
			//btTriangleIndexVertexArray meshShape(numTriangles, indexPointer, indexStride, numVertices, vertexPointer, vertexStride);
			//btCollisionShape* shape = btBvhTriangleMeshShape(meshShape, true);

			//_body = createBulletRigidBody(shape, mass, node, friction, restitution, linearDamping, angularDamping);
			break;
		}
		case PhysicsRigidBody::PHYSICS_SHAPE_HEIGHTFIELD:
		{
			//btCollisionShape* shape = btHeightfieldTerrainShape(width, length, data, scale, minHeight, maxHeight, upAxis, dataType, false);

			//_body = createBulletRigidBody(shape, mass, node, friction, restitution, linearDamping, angularDamping);
			break;
		}
	}
}

PhysicsRigidBody::~PhysicsRigidBody()
{
    if (_body)
    {
        if (_body->getMotionState())
            delete _body->getMotionState();

        delete _body;
    }
}

void PhysicsRigidBody::applyForce(const Vector3& force, const Vector3* relativePosition)
{
    // If the force is significant enough, activate the rigid body 
    // to make sure that it isn't sleeping and apply the force.
    if (force.lengthSquared() > MATH_EPSILON)
    {
        _body->activate();
        if (relativePosition)
		    _body->applyForce(btVector3(force.x, force.y, force.z), btVector3(relativePosition->x, relativePosition->y, relativePosition->z));
	    else
		    _body->applyCentralForce(btVector3(force.x, force.y, force.z));
    }
}

void PhysicsRigidBody::applyImpulse(const Vector3& impulse, const Vector3* relativePosition)
{
    // If the impulse is significant enough, activate the rigid body 
    // to make sure that it isn't sleeping and apply the impulse.
    if (impulse.lengthSquared() > MATH_EPSILON)
    {
        _body->activate();

	    if (relativePosition)
        {
		    _body->applyImpulse(btVector3(impulse.x, impulse.y, impulse.z), btVector3(relativePosition->x, relativePosition->y, relativePosition->z));
        }
	    else
		    _body->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
    }
}

void PhysicsRigidBody::applyTorque(const Vector3& torque)
{
    // If the torque is significant enough, activate the rigid body 
    // to make sure that it isn't sleeping and apply the torque.
    if (torque.lengthSquared() > MATH_EPSILON)
    {
        _body->activate();
	    _body->applyTorque(btVector3(torque.x, torque.y, torque.z));
    }
}

void PhysicsRigidBody::applyTorqueImpulse(const Vector3& torque)
{
    // If the torque impulse is significant enough, activate the rigid body 
    // to make sure that it isn't sleeping and apply the torque impulse.
    if (torque.lengthSquared() > MATH_EPSILON)
    {
        _body->activate();
        _body->applyTorqueImpulse(btVector3(torque.x, torque.y, torque.z));
    }
}

btRigidBody* PhysicsRigidBody::createBulletRigidBody(btCollisionShape* shape, float mass, Node* node,
    float friction, float restitution, float linearDamping, float angularDamping)
{
	// If the mass is non-zero, then the object is dynamic
	// and we need to calculate the local inertia.
	btVector3 localInertia(0.0, 0.0, 0.0);
	if (mass != 0.0)
		shape->calculateLocalInertia(mass, localInertia);

	// Create the Bullet physics rigid body object.
	PhysicsMotionState* motionState = new PhysicsMotionState(node);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    rbInfo.m_friction = friction;
    rbInfo.m_restitution = restitution;
    rbInfo.m_linearDamping = linearDamping;
    rbInfo.m_angularDamping = angularDamping;
	btRigidBody* body = new btRigidBody(rbInfo);

	// Add the rigid body to the physics world.
	PhysicsController* physics = Game::getInstance()->getPhysicsController();
	physics->_world->addRigidBody(body);

	return body;
}

}
