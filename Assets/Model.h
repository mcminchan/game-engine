#ifndef PROJECT1_MODEL_H
#define PROJECT1_MODEL_H

#include "Mesh.h"
#include "Shader.h"
#include "Light.h"
#include "RigidBody.h"
#include <PxPhysicsAPI.h>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace physx;
class Model
{
public:

	struct Rotation
	{
		float x;
		float y;
		float z;
	};

	struct Transform
	{
		PxVec3 position;
		PxVec3 rotation;
		PxVec3 scale;
	};

	struct Env
	{
		bool cullable;
		bool isRenderCollider;
		bool viewNormal;
	};

	Shader* currentShader;
	Transform transform;
	Env env;
	int mLightIndex;
	Light::LightType mLightType;
	RigidBody mRigidBody;

	Model();
	Model(const char* path);
	Model(Mesh singleMesh);
	Model(unsigned int VAO, unsigned _int64 indicesSize);

	void setDiffuse(Texture texture);
	void setDiffuse(const char* fileName, std::string directory);
	void draw();
	void draw(const Shader& shader);
	//void drawSelected(Shader& objShader, Shader& borderShader);
	void setRigidDynamic();
	void setCollider(const physx::PxGeometry& collider);
	//void getBoxRigidStatic(const glm::vec3& size);
	void setPosition(const PxVec3& position);
	void setRotation(const PxVec3& rotation);
	void setTransform(const PxTransform& transform);

	void removeCollider();
	void removeLight();

	void addLight(const Light::PointLight& light);
	void setLight(const Light::PointLight& light);
	void addLight(const Light::SpotLight& light);
	void setLight(const Light::SpotLight& light);
	void subOneLightIndex(unsigned int pivot);

	void setViewNormal();
	const char* getTextureName();
	int getShaderNum() const;
	PxMat44 getTransformMat() const;
	void setShader(Shader* shader, int shaderNum);
	void cull();

private:

	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	int shaderNum;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

};
#endif