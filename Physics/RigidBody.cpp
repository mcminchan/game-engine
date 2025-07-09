#include "Physics.h"
#include "RigidBody.h"
#include "Camera.h"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace physx;

RigidBody::RigidBody()
	: mCollider{}
	, mRigidActor{ NULL }
	,mIsRigidBody{false}
	,mIsCollider{false}
	,mActorType{PxActorType::eRIGID_STATIC}
{
	
}

void RigidBody::setDynamic()
{
	mActorType = PxActorType::eRIGID_DYNAMIC;
}

void RigidBody::setStatic()
{
	mActorType = PxActorType::eRIGID_STATIC;
}
void RigidBody::setCollider(const PxGeometry& collider)
{
	mCollider.storeAny(collider);
	mIsCollider = true;
}
void RigidBody::setTransform(const PxTransform& transform)
{
	if (mIsRigidBody)
		mRigidActor->setGlobalPose(transform);
}

void RigidBody::setRigidActor(PxRigidActor* rigidActor)
{
	mRigidActor = rigidActor;
	mIsRigidBody = true;
}

void RigidBody::removeRigidBody()
{
	mRigidActor = NULL;
	mIsRigidBody = false;
}

void RigidBody::removeCollider()
{
	mIsCollider = false;
}
bool RigidBody::isDynamic() const
{
	if (mActorType == PxActorType::eRIGID_DYNAMIC)
		return true;
	else
		return false;
}

bool RigidBody::isCollider() const
{
	if (mIsCollider)
		return true;
	else
		return false;
}

bool RigidBody::isRigidBody() const
{
	return mIsRigidBody;
}

void RigidBody::renderCollider( PxMat44& model) const
{
	const Shader& shader = Physics::getColliderShader();
	shader.use();
	if (mCollider.getType()==PxGeometryType::eBOX)
	{
		PxVec3 size = mCollider.box().halfExtents;
		model.scale(PxVec4(size.x, size.y, size.z, 1));
		Camera::setModelMat(shader, model);

		glDepthFunc(GL_NOTEQUAL);
		extern void renderCube();
		renderCube();
		glDepthFunc(GL_LESS);
	}
}

PxTransform RigidBody::getTransform() const
{
	return mRigidActor->getGlobalPose();
}

PxGeometryHolder& RigidBody::getCollider() 
{
	
	return mCollider;
}

PxActorType::Enum RigidBody::getActorType() const
{
	return mActorType;
}