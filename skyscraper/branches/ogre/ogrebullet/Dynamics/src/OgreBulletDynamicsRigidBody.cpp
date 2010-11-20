/***************************************************************************

This source file is part of OGREBULLET
(Object-oriented Graphics Rendering Engine Bullet Wrapper)
For the latest info, see http://www.ogre3d.org/phpBB2addons/viewforum.php?f=10

Copyright (c) 2007 tuan.kuranes@gmail.com (Use it Freely, even Statically, but have to contribute any changes)



Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "OgreBulletDynamics.h"

#include "OgreBulletCollisionsShape.h"
#include "OgreBulletCollisionsObject.h"
#include "OgreBulletCollisionsWorld.h"
#include "OgreBulletCollisionsObjectState.h"

#include "OgreBulletDynamicsWorld.h"
#include "OgreBulletDynamicsRigidBody.h"
#include "OgreBulletDynamicsObjectState.h"

using namespace Ogre;
using namespace OgreBulletCollisions;

namespace OgreBulletDynamics
{

    // -------------------------------------------------------------------------
    RigidBody::RigidBody(const String &name, DynamicsWorld *world, const short collisionGroup, const short collisionMask)
        :	
        Object(name, world, false)
    {
		mCollisionGroup = collisionGroup;
		mCollisionMask = collisionMask;
    }
    // -------------------------------------------------------------------------
    RigidBody::~RigidBody()
    {
    }  
    // -------------------------------------------------------------------------
    void RigidBody::setShape(Ogre::SceneNode *node,  
        OgreBulletCollisions::CollisionShape *shape,                          
        const float      bodyRestitution,
        const float      bodyFriction,
        const float      bodyMass,
        const Vector3 &pos, 
        const Quaternion &quat)
    {

        mState = new ObjectState(this);

        mRootNode = node;
        mShapeNode = mRootNode->createChildSceneNode(mName + "Node");
        mShapeNode->attachObject(this);

        node->setPosition (pos);
        node->setOrientation (quat);

        mShape = shape;
        showDebugShape(mWorld->getShowDebugShapes());

        btVector3 localInertiaTensor = btVector3(0,0,0);
		if (bodyMass > 0.0)
	        mShape->getBulletShape ()->calculateLocalInertia(bodyMass, localInertiaTensor);

        btRigidBody *body = new btRigidBody(bodyMass, mState, mShape->getBulletShape (), localInertiaTensor);
        body->setRestitution(bodyRestitution);
        body->setFriction(bodyFriction);

        mObject = body;

		getDynamicsWorld()->addRigidBody(this, mCollisionGroup, mCollisionMask);
    }

    // -------------------------------------------------------------------------
    void RigidBody::setStaticShape(Ogre::SceneNode *node, 
        OgreBulletCollisions::CollisionShape *shape,
        const float      bodyRestitution,
        const float      bodyFriction,
        const Vector3 &pos, 
        const Quaternion &quat)
    {
        mState = new ObjectState(this);

        mRootNode = node;

        mShapeNode = mRootNode->createChildSceneNode(mName + "Node");
        mShapeNode->attachObject(this);

        node->setPosition (pos);
        node->setOrientation (quat);

        mShape = shape;
        showDebugShape(mWorld->getShowDebugShapes());

        btRigidBody *body = new btRigidBody(0.0, mState, mShape->getBulletShape ());

        body->setRestitution(bodyRestitution);
        body->setFriction(bodyFriction);

        body->getWorldTransform().setOrigin(btVector3(pos.x, pos.y, pos.z));
        body->getWorldTransform().setRotation(btQuaternion(quat.x, quat.y, quat.z, quat.w));

        mObject = body;
		getDynamicsWorld()->addRigidBody(this, mCollisionGroup, mCollisionMask);
	}
	// -------------------------------------------------------------------------
    void RigidBody::setStaticShape(btScaledBvhTriangleMeshShape *shape,
        const float      bodyRestitution,
        const float      bodyFriction,
        const Vector3 &pos,
        const Quaternion &quat)
    {
        //mState = new ObjectState(this);

        btRigidBody *body = new btRigidBody(0.0, 0, shape);

        body->setRestitution(bodyRestitution);
        body->setFriction(bodyFriction);

        body->getWorldTransform().setOrigin(btVector3(pos.x, pos.y, pos.z));
        body->getWorldTransform().setRotation(btQuaternion(quat.x, quat.y, quat.z, quat.w));

        mObject = body;
      getDynamicsWorld()->addRigidBody(this, mCollisionGroup, mCollisionMask);
   }		
    // -------------------------------------------------------------------------
    void RigidBody::setStaticShape(OgreBulletCollisions::CollisionShape *shape,
        const float      bodyRestitution,
        const float      bodyFriction,
        const Vector3 &pos, 
        const Quaternion &quat)
    {
        //mState = new ObjectState(this);

        mShape = shape;
        btRigidBody *body = new btRigidBody(0.0, 0, mShape->getBulletShape ());

        body->setRestitution(bodyRestitution);
        body->setFriction(bodyFriction);

        body->getWorldTransform().setOrigin(btVector3(pos.x, pos.y, pos.z));
        body->getWorldTransform().setRotation(btQuaternion(quat.x, quat.y, quat.z, quat.w));

        mObject = body;
		getDynamicsWorld()->addRigidBody(this, mCollisionGroup, mCollisionMask);
	}
	// -------------------------------------------------------------------------
	void RigidBody::setKinematicObject(bool isKinematic) {
		if (this->isKinematicObject() != isKinematic) {
			//flip kinematic state
			getBulletRigidBody()->setCollisionFlags(
				getBulletRigidBody()->getCollisionFlags() ^ btCollisionObject::CF_KINEMATIC_OBJECT);
		}
	} 
    // -------------------------------------------------------------------------
    void RigidBody::setLinearVelocity( const Ogre::Vector3 &vel )
    {
        getBulletRigidBody()->setLinearVelocity (btVector3(vel.x, vel.y, vel.z));
    }
    // -------------------------------------------------------------------------
    void RigidBody::setLinearVelocity( const Ogre::Real x, const Ogre::Real y, const Ogre::Real z )
    {
        getBulletRigidBody()->setLinearVelocity (btVector3(x, y, z));
    }
    // -------------------------------------------------------------------------
	Ogre::Vector3 RigidBody::getLinearVelocity()
	{
		const btVector3 lv = getBulletRigidBody()->getLinearVelocity();
		return BtOgreConverter::to(lv);
	}
    // -------------------------------------------------------------------------
    void RigidBody::applyImpulse( const Ogre::Vector3 &impulse, const Ogre::Vector3 &position )
    {
        getBulletRigidBody()->applyImpulse (OgreBtConverter::to(impulse), OgreBtConverter::to(position));
    }
    // -------------------------------------------------------------------------
    void RigidBody::applyForce( const Ogre::Vector3 &impulse, const Ogre::Vector3 &position )
    {
        getBulletRigidBody()->applyForce(OgreBtConverter::to(impulse), OgreBtConverter::to(position));
    }
    // -------------------------------------------------------------------------
    Ogre::Vector3 RigidBody::getCenterOfMassPivot( const Ogre::Vector3 &pivotPosition ) const
    {
        const btVector3 centerOfMassPivot(getCenterOfMassTransform().inverse()* OgreBtConverter::to(pivotPosition));
        return BtOgreConverter::to(centerOfMassPivot);
    }
    // -------------------------------------------------------------------------
    void RigidBody::setDeactivationTime( const float ftime )
    {
        getBulletRigidBody()->setDeactivationTime( ftime );
    }
    // -------------------------------------------------------------------------
    void RigidBody::setDamping( const Ogre::Real linearDamping, const Ogre::Real angularDamping )
    {
        getBulletRigidBody()->setDamping( linearDamping,  angularDamping);
    }
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    void WheeledRigidBody::setPosition(const btVector3 &pos)
    { 
        //should update wheels as well ?
        mRootNode->setPosition(pos[0], pos[1], pos[2]);
    };
    // -------------------------------------------------------------------------
    void WheeledRigidBody::setOrientation(const btQuaternion &quat)
    { 
        mRootNode->setOrientation(quat.getW(),quat.getX(), quat.getY(), quat.getZ());
    };
    // -------------------------------------------------------------------------
    void WheeledRigidBody::setTransform(const btVector3 &pos, const btQuaternion &quat)
    {
        mRootNode->setPosition(pos[0], pos[1], pos[2]);
        mRootNode->setOrientation(quat.getW(),quat.getX(), quat.getY(), quat.getZ());

        mVehicle->setTransform();
    }
    // -------------------------------------------------------------------------
    void WheeledRigidBody::setTransform(const btTransform& worldTrans)
    { 
        mRootNode->setPosition(worldTrans.getOrigin()[0], worldTrans.getOrigin()[1],worldTrans.getOrigin()[2]);
        mRootNode->setOrientation(worldTrans.getRotation().getW(),worldTrans.getRotation().getX(), worldTrans.getRotation().getY(), worldTrans.getRotation().getZ());

        mVehicle->setTransform();
    }
}

