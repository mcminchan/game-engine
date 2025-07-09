#include "stb_image.h"
#include "Model.h"
#include "Shader.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "Camera.h"
#include "Physics.h"
#include "RigidBody.h"
#include "TextureLoader.h"
#include "Light.h"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <string>
#include <vector>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace physx;
Model::Model()
{}
Model::Model(const char* path)
	:transform{ {0,0,0},{0,0,0},{1,1,1} }
	, currentShader{ NULL }
	, shaderNum{ 0 }
	, env{ false }
	, mRigidBody{ }
	, mLightIndex{ -1 }
	, mLightType{Light::none}
{

	loadModel(path);
}
Model::Model(Mesh singleMesh)
	:meshes{ {singleMesh} }
	, transform{ {0,0,0},{0,0,0},{1,1,1} }
	, currentShader{}
	, shaderNum{ 0 }
	, env{ false }
	, mRigidBody{ }
	, mLightIndex{ -1 }
	, mLightType{ Light::none }
{
}
Model::Model(unsigned int VAO, unsigned _int64 indicesSize)
	:transform{ {0,0,0},{0,0,0},{1,1,1} }
	, currentShader{ NULL }
	, shaderNum{ 0 }
	, env{ false }
	, mRigidBody{ }
	, mLightIndex{ -1 }
	, mLightType{ Light::none }
{
	meshes.push_back(Mesh(VAO, indicesSize));
}

void Model::setDiffuse(Texture texture)
{
	for (int a = 0; a < meshes.size(); a++)
		meshes[a].setDiffuse(texture);
}
void Model::setDiffuse(const char* fileName, std::string directory)
{
	unsigned int textureID = TextureLoader::get2DTextureId((directory + '/' + fileName).c_str());

	setDiffuse(Texture{ textureID,"texture_diffuse" ,fileName });
}
void Model::draw()
{
	currentShader->use();
	if (env.cullable)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	PxMat44 model;
	if (mRigidBody.isRigidBody())
	{
		PxTransform t{ mRigidBody.getTransform() };
		setTransform(t);
		PxMat44 modelMat{ t };
		model = modelMat;
		modelMat.scale(PxVec4(transform.scale.x, transform.scale.y, transform.scale.z, 1.0f));
		Camera::setModelMat(*currentShader, modelMat);
		
	}
	else
	{
		PxMat44 modelMat{ getTransformMat() };
		model= modelMat;
		modelMat.scale(PxVec4(transform.scale, 1.0f));
		Camera::setModelMat(*currentShader, modelMat);
	}

	for (int a = 0; a < meshes.size(); a++)
	{
		meshes[a].draw(*currentShader);
	}

	if (env.isRenderCollider)
		mRigidBody.renderCollider(model);
	if (mLightType != Light::none)
	{
		Light::updateLightPosition(mLightIndex, mLightType, transform.position);
		if (mLightType == Light::spotLight)
			Light::updateLightRotation(mLightIndex, transform.rotation);
	}
	if (env.viewNormal)
	{
		Shader* temp = currentShader;
		env.viewNormal = false;
		currentShader = &ShaderManager::getShader(ShaderManager::normalTest);
		draw();
		currentShader = temp;
		env.viewNormal = true;
	}
}

void Model::draw(const Shader& shader)
{
	shader.use();

	PxMat44 model;
	if (mRigidBody.isRigidBody())
	{
		PxTransform t{ mRigidBody.getTransform() };
		setTransform(t);
		PxMat44 modelMat{ t };
		model = modelMat;
		modelMat.scale(PxVec4(transform.scale.x, transform.scale.y, transform.scale.z, 1.0f));
		shader.setUniformMat4("model", modelMat);

	}
	else
	{
		PxMat44 modelMat{ getTransformMat() };
		model = modelMat;
		modelMat.scale(PxVec4(transform.scale, 1.0f));
		shader.setUniformMat4("model", modelMat);
	}

	for (int a = 0; a < meshes.size(); a++)
	{
		meshes[a].draw(shader);
	}
}

//void Model::drawSelected(Shader& objShader, Shader& borderShader)
//{
//	glStencilMask(0xff);
//	glStencilFunc(GL_ALWAYS, 1, 0xff);
//	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//
//	draw(objShader,cam);
//	glStencilFunc(GL_EQUAL, 0, 0xff);
//	glStencilOp(GL_REPLACE, GL_KEEP,GL_KEEP);
//	glDisable(GL_DEPTH_TEST);
//	glm::vec3 scale = transform.scale * 1.05f;
//	draw(borderShader,cam, transform.position,transform.angle,transform.rotation,scale);
//	glStencilMask(0);
//	glStencilFunc(GL_ALWAYS, 0, 0xff);
//	glEnable(GL_DEPTH_TEST);
//
//}

const char* Model::getTextureName()
{
	if (meshes[0].textures.size() < 1)
		return NULL;
	std::string& fileName = meshes[0].textures[0].path;
	if (fileName.empty())
		return NULL;
	else
		return fileName.c_str();
}

void Model::setShader(Shader* shader, int shaderNum)
{
	currentShader = shader;
	Model::shaderNum = shaderNum;
}

void Model::setTransform(const PxTransform& Ptransform)
{
	transform.position = Ptransform.p;
	glm::quat quat(Ptransform.q.w, Ptransform.q.x, Ptransform.q.y, Ptransform.q.z);
	glm::vec3 rot{ glm::degrees(glm::eulerAngles(quat)) };

	transform.rotation = PxVec3{ rot.x,rot.y,rot.z };
}
void Model::setPosition(const PxVec3& position)
{
	if (mRigidBody.isRigidBody())
	{
		mRigidBody.setTransform(PxTransform(transform.position,mRigidBody.getTransform().q));
	}
	else
		transform.position = position;
	

};

void Model::setRotation(const PxVec3& rotation)
{
	if (mRigidBody.isRigidBody())
	{
		PxQuat x= PxGetRotXQuat(PxDegToRad(rotation.x));
		PxQuat y = PxGetRotYQuat(PxDegToRad(rotation.y));
		PxQuat z = PxGetRotZQuat(PxDegToRad(rotation.z));
		mRigidBody.setTransform(PxTransform{ transform.position,z * y * x }); //x->y->z순 회전
	}
	else
		transform.rotation = rotation;
}

void Model::addLight(const Light::PointLight& pointLight)
{
	if (mLightType == Light::none)
	{
		mLightType = Light::pointLight;
		mLightIndex = Light::addLight(pointLight);
	}
}
void Model::addLight(const Light::SpotLight& spotLight)
{
	if (mLightType == Light::none)
	{
		mLightType = Light::spotLight;
		mLightIndex = Light::addLight(spotLight);
	}
}

void Model::setLight(const Light::PointLight& pointLight)
{
	Light::updateLight(pointLight, mLightIndex);
}

void Model::setLight(const Light::SpotLight& spotLight)
{
	Light::updateLight(spotLight, mLightIndex);
}

void Model::subOneLightIndex(unsigned int pivot)
{
	if (mLightIndex > pivot)
		mLightIndex--;
}

void Model::setViewNormal()
{
	env.viewNormal = true;
}

void Model::cull()
{
	env.cullable = true;
}

int Model::getShaderNum() const
{
	return shaderNum;
}

void Model::setRigidDynamic()
{
	if (!mRigidBody.isDynamic())
		mRigidBody.setDynamic();	//model에게 rigidbody는 scene이 시작되고 주기때문에 아직 설정하지 않는다.
	
}

void Model::setCollider(const physx::PxGeometry& collider)
{

	if (!mRigidBody.isCollider())
	{
		mRigidBody.setCollider(collider);
		Physics::addActor(this);
	}
}

void Model::removeCollider() 
{
	mRigidBody.removeCollider();
	Physics::removeActor(this);
}

void Model::removeLight()
{
	Light::removeLight(mLightIndex, mLightType);
	mLightIndex = -1;
	mLightType = Light::none;
}

PxMat44 Model::getTransformMat() const
{
	PxMat33 rotationMat;
	PxMat33 x, y, z;

	PxSetRotX(x, PxDegToRad(transform.rotation.x));
	PxSetRotY(y, PxDegToRad(transform.rotation.y));
	PxSetRotZ(z, PxDegToRad(transform.rotation.z));
	rotationMat = z * y * x;					//x->y->z순 회전
	return PxMat44{ rotationMat,transform.position };
	
}
//void Model::getBoxRigidStatic(const glm::vec3& size)
//{
//	isRigidBody = true;
//	glm::quat quatRotation = glm::quat(transform.rotation);
//	PxTransform gTransform(PxVec3(transform.position.x, transform.position.y, transform.position.z), PxQuat(quatRotation.w, quatRotation.x, quatRotation.y, quatRotation.z));
//	rigidBody = static_cast<PxRigidBody*>(addStatic(gTransform, PxBoxGeometry(size.x, size.y, size.z)));
//}

///////////////////////private//////////////////////


void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		std::cout << path << ": " << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}
void Model::processNode(aiNode* node, const aiScene* scene)	
{
	for (int a = 0; a < node->mNumMeshes; a++)
	{
		meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[a]], scene));
	}
	for (int a = 0; a < node->mNumChildren; a++)
	{
		processNode(node->mChildren[a], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices{};
	std::vector<unsigned int> indices{};
	std::vector<Texture> textures{};

	Vertex temp;
	for (int a = 0; a < mesh->mNumVertices; a++)
	{
		temp.position.x = mesh->mVertices[a].x;
		temp.position.y = mesh->mVertices[a].y;
		temp.position.z = mesh->mVertices[a].z;
		temp.normal.x = mesh->mNormals[a].x;
		temp.normal.y = mesh->mNormals[a].y;
		temp.normal.z = mesh->mNormals[a].z;
		if (mesh->mTextureCoords[0])
		{
			temp.texCoord.x = mesh->mTextureCoords[0][a].x;
			temp.texCoord.y = mesh->mTextureCoords[0][a].y;
		}
		else
			temp.texCoord = glm::vec2(0, 0);
		vertices.push_back(temp);
	}

	aiFace faceTemp;
	for (int a = 0; a < mesh->mNumFaces; a++)
	{
		faceTemp = mesh->mFaces[a];
		for (int b = 0; b < faceTemp.mNumIndices; b++)
			indices.push_back(faceTemp.mIndices[b]);
	}

	aiMaterial* materialTemp;
	if (mesh->mMaterialIndex >= 0)
	{
		materialTemp = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMap = loadMaterialTextures(materialTemp, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMap.begin(), diffuseMap.end());
		std::vector<Texture> specularMap = loadMaterialTextures(materialTemp, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMap.begin(), specularMap.end());
	}
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
	aiString location;
	std::vector<Texture> temp{};
	Texture texture;
	bool skip = false;
	for (int a = 0; a < material->GetTextureCount(type); a++)
	{
		material->GetTexture(type, a, &location);
		for (int b = 0; b < textures_loaded.size(); b++)
			if (textures_loaded[b].path.compare(location.C_Str()) == 0)
			{
				skip = true;
				temp.push_back(textures_loaded[b]);
				break;
			}
		if (!skip)
		{
			const char* path = location.C_Str();
			texture.id = TextureLoader::get2DTextureId((directory + '/' + path).c_str());
			texture.type = typeName;
			texture.path = path;
			textures_loaded.push_back(texture);
			temp.push_back(texture);
		}
	}
	return temp;
}