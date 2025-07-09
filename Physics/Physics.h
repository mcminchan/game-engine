#ifndef PROJECT1_PHYSICS_H
#define PROJECT1_PHYSICS_H
#include "Model.h"
#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>

using namespace physx;


namespace Physics
{
	extern void initPhysics();

	extern PxRigidDynamic* addDynamic(const PxTransform& transform, const PxGeometry& geometry);
	extern PxRigidStatic* addStatic(const PxTransform& transform, const PxGeometry& geometry);
	extern void addActor(Model* model);

	extern void removeActor(Model* model);

	extern void stepPhysics(float time);
	extern void setupScene();
	extern void clearScene();

	extern PxShape* createBoxShape(const PxVec3& size);
	extern const Shader& getColliderShader();

	extern void freePhysics();
}
#endif