#ifndef PROJECT1_RIGIDBODY_H
#define PROJECT1_RIGIDBODY_H

#include "Shader.h"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;
class RigidBody
{
	PxGeometryHolder mCollider;
	PxRigidActor* mRigidActor;
	PxActorType::Enum mActorType;
	bool mIsCollider;
	bool mIsRigidBody;		//true if physics is simulated


public:

	RigidBody();
	void setDynamic();
	void setStatic();
	void setCollider(const PxGeometry& collider);
	void setTransform(const PxTransform& transform);
	void setRigidActor(PxRigidActor* rigidActor);

	void removeRigidBody();
	void removeCollider();

	void renderCollider(PxMat44& model) const;

	bool isDynamic() const;
	bool isCollider() const;
	bool isRigidBody() const;

	PxTransform getTransform() const;
	PxGeometryHolder& getCollider();
	PxActorType::Enum getActorType() const;
};

#endif
