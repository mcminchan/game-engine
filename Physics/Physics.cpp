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
		//�⺻���� ���� : foundation, physics, scene. 

		static PxDefaultAllocator pxAllocatorCallback;
		static PxDefaultErrorCallback pxErrorCallback;
		gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, pxAllocatorCallback, pxErrorCallback); //foundation ����, �޸𸮴� �ý��۸��� �������� �ٸ��Ƿ� ���� �ݹ� ����
		if (!gFoundation)
		{
			std::cout << "foundation error" << std::endl;
			exit(EXIT_FAILURE);
		}

		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());		//foundation�� �������� physics ����, toleranceScale�� ������ �̱� �ѱ� ���� ���� �� �ٸ�.
		if (!gPhysics)
		{
			std::cout << "physics creation error" << std::endl;
			exit(EXIT_FAILURE);
		}

		PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);			//�Ʒ� �ΰ��� document �о��
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

		gScene = gPhysics->createScene(sceneDesc);		//phyiscs�� �������� scene ����, scene�� ���� ������ sceneDesc�� �̿��Ͽ� ����.

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
			���ڴ� ridig�� ���� ����� shape attach, ���ڴ� �ѹ��� ����, �Ʒ��� �ڵ�� shape�� �ѹ��� ����
		*/
		PxRigidDynamic* object = PxCreateDynamic(*gPhysics, transform, geometry, *gMaterial, 10.0f);
		dynamicActors.push_back(object);
		return object;
	};
	PxRigidStatic* addStatic(const PxTransform& transform, const PxGeometry& geometry)
	{
		/*
		* static�̶� mass�� �ӵ� ���� �������� �ʴ´ٴ� �͸� �����ϰ� ���̳��Ͱ� ����.
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
		gScene->fetchResults(true);		//true�� result ���������� ��ٸ�.
	}

	void freePhysics()
	{
		gScene->release();
		gPhysics->release();
		gFoundation->release();
	}
}