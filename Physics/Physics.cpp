#include "Physics.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include <iostream>
#include <vector>
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <foundation/PxAllocatorCallback.h>
#include <foundation/PxErrorCallback.h>

using namespace physx;
static Shader mColliderShader;
static PxFoundation* gFoundation = NULL;
static PxPhysics* gPhysics = NULL;
static PxScene* gScene = NULL;
static PxMaterial* gMaterial = NULL;
static std::vector<PxTransform> dynamicsTransform;
static std::vector<PxRigidDynamic*> dynamicActors;
static std::vector<Model*> models;

namespace Camera
{
	extern void setModelMat(Shader& shader, const glm::mat4& model);
}
namespace Physics
{

	void initPhysics()
	{
		//기본적인 구조 : foundation, physics, scene. 

		static PxDefaultAllocator pxAllocatorCallback;
		static PxDefaultErrorCallback pxErrorCallback;
		gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, pxAllocatorCallback, pxErrorCallback); //foundation 생성, 메모리는 시스템마다 관리법이 다르므로 따로 콜백 지정
		if (!gFoundation)
		{
			std::cout << "foundation error" << std::endl;
			exit(EXIT_FAILURE);
		}

		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());		//foundation을 바탕으로 physics 생성, toleranceScale은 단위임 미국 한국 미터 단위 다 다름.
		if (!gPhysics)
		{
			std::cout << "physics creation error" << std::endl;
			exit(EXIT_FAILURE);
		}

		PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);			//아래 두개는 document 읽어보기
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

		gScene = gPhysics->createScene(sceneDesc);		//phyiscs를 바탕으로 scene 생성, scene에 대한 정보는 sceneDesc를 이용하여 얻음.

		gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.5f);

		mColliderShader = Shader{ "ShaderSources/borderShader.vs","ShaderSources/borderShader.fs" };
	};

	PxRigidDynamic* addDynamic(const PxTransform& transform, const PxGeometry& geometry)
	{

		/*
			PxShape* shape = gPhysics->createShape(geometry, *gMaterial);
			PxRigidDynamic* object= PxCreateDynamic(*gPhysics,transform,*shape,10.0f);
			or
			PxRigidDynamic* object= gPhysics->PxCreateRigidDynamic(transform);
			object->attachShape(*shape)
			object->setMass(10.0f); or PxRigidBodyExt::updateMassAndInertia(*object, 10.0f);
			후자는 ridig만 따로 만들고 shape attach, 전자는 한번에 만듦, 아래의 코드는 shape도 한번에 만듦
		*/
		PxRigidDynamic* object = PxCreateDynamic(*gPhysics, transform, geometry, *gMaterial, 10.0f);
		dynamicActors.push_back(object);
		return object;
	};
	PxRigidStatic* addStatic(const PxTransform& transform, const PxGeometry& geometry)
	{
		/*
		* static이라서 mass와 속도 등이 존재하지 않는다는 것만 제외하고 다이나믹과 같음.
		*/
		PxRigidStatic* object = PxCreateStatic(*gPhysics, transform, geometry, *gMaterial);
		return object;
	};

	void addActor(Model* model)
	{
		models.push_back(model);
	}

	void removeActor(Model* model)
	{
		bool removed = false;
		for (auto a = models.begin(); a != models.end();)
		{
			if (model == (*a))
			{

				a = models.erase(a);
				removed = true;
			}
			else
				a++;
		}
		if (!removed)
			std::cout << "not exist a model to remove" << std::endl;
	}

	PxShape* createBoxShape(const PxVec3& size)
	{
		return gPhysics->createShape(PxBoxGeometry(size), *gMaterial);
	}


	void setupScene()
	{
		dynamicsTransform.resize(models.size());
		for (int a = 0; a < models.size(); a++)
		{
			Model* model = models.at(a);
			if (model->mRigidBody.isCollider())
			{
				PxTransform transform{ model->getTransformMat() };
				dynamicsTransform.at(a) = transform;

				PxRigidActor* actor;
				if (model->mRigidBody.isDynamic())
					actor = addDynamic(transform, model->mRigidBody.getCollider().any());
				else
					actor = addStatic(transform, model->mRigidBody.getCollider().any());
				model->mRigidBody.setRigidActor(actor);

				gScene->addActor(*actor);
				if (actor->getType() == PxActorType::eRIGID_DYNAMIC)
					static_cast<PxRigidDynamic*>(actor)->wakeUp();
			}
		}
	}

	void clearScene()
	{
		PxActor* actors[10];
		PxActorTypeFlags flags;
		flags.raise(PxActorTypeFlag::eRIGID_DYNAMIC);
		flags.raise(PxActorTypeFlag::eRIGID_STATIC);
		gScene->getActors(flags, actors, gScene->getNbActors(flags));

		gScene->removeActors(actors, gScene->getNbActors(flags));
		for (int a = 0; a < models.size(); a++)
		{
			models.at(a)->setTransform(dynamicsTransform.at(a));
			models.at(a)->mRigidBody.removeRigidBody();
		}
	}

	const Shader& getColliderShader()
	{
		return mColliderShader;
	}

	void stepPhysics(float time)
	{
		gScene->simulate(time);
		gScene->fetchResults(true);		//true면 result 얻을때까지 기다림.
	}

	void freePhysics()
	{
		gScene->release();
		gPhysics->release();
		gFoundation->release();
	}
}