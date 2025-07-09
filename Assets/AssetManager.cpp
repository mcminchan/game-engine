#include "Shader.h"
#include "ShaderManager.h"
#include "AssetManager.h"
#include "TextureLoader.h"
#include "Buffers.h"
#include "Primitives.h"
#include "Physics.h"
#include "Model.h"
#include "Light.h"
#include <map>
#include <vector>
#include <glm/glm.hpp>

namespace AssetManager
{
	std::map<std::string, Model> gameObjects{};
	std::map<std::string, Model> transparentGameObjects{};
	Model frame;
	Model skybox;

	void initGameObjects()
	{

		Light::setupUniformBuffer();
		Light::updateLight(Light::DirectionalLight{ glm::vec3(1,1,1)
			,glm::vec3(0.5f,0.5f,0.5f)
			,glm::vec3(0.5f,0.5f,0.5f)
			,glm::vec3(0.1f,0.1f,0.1f) });
		
		std::vector<std::string> facePath{ {"skybox/right.png"},{"skybox/left.png"} ,{"skybox/top.png"} ,{"skybox/bottom.png"} ,{"skybox/back.png"} ,{"skybox/front.png"} };

		unsigned int grassTex = TextureLoader::get2DTextureId("prefab/grass.png");
		unsigned int skyboxTex = TextureLoader::getSkyboxTexture(facePath);

		frame = Model{ getRectVAO() ,getRectEBOsize() };
		frame.setShader(&ShaderManager::getShader(ShaderManager::frame), ShaderManager::frame);
		frame.transform.scale.y = 0.5f;

		skybox=Model{ Mesh{getCubeVAO(),getCubeEBOsize(),std::vector<Texture>{{skyboxTex,"texture_diffuse"}}} };
		skybox.setShader(&ShaderManager::getShader(ShaderManager::skybox), ShaderManager::frame);
		/*gameObjects["mirror"] = Model{ getRectVAO() ,getRectEBOsize() };
		Model& mirror = gameObjects["mirror"];
		mirror.transform.position = glm::vec3(0, 0, -3);
		mirror.setShader(&mirrorShader, 4);*/

		gameObjects["grass"] = Model{ Mesh{getRectVAO() ,getRectEBOsize(),std::vector<Texture>{{grassTex,"texture_diffuse"}}} };
		gameObjects["grass"].transform.position = PxVec3{ 1,1,-1 };
		gameObjects["grass"].setShader(&ShaderManager::getShader(ShaderManager::basic), ShaderManager::basic);

		gameObjects["light"] = Model{ getCubeVAO(),getCubeEBOsize() };
		gameObjects["light"].setShader(&ShaderManager::getShader(ShaderManager::light), ShaderManager::light);
		gameObjects["light"].cull();
		gameObjects["light"].addLight(Light::PointLight{ glm::vec3(0.0f,0.0f,0.0f),	//position
			glm::vec3(1.0f,1.0f,1.0f),			//diffuse
			glm::vec3(0.5f,0.5f,0.5f),			//specular
			glm::vec3(0.2f,0.2f,0.2f),			//ambient
			1.0, 0.22, 0.20 });					//constant linear quadratic

		gameObjects["camera"] = Model{ "prefab/camera.obj" };
		Model& camera = gameObjects["camera"];
		camera.transform.position.x = -5;
		camera.setShader(&ShaderManager::getShader(ShaderManager::light), ShaderManager::light);
		camera.cull();

		gameObjects["donut"] = Model{ "prefab/donut.obj" };
		gameObjects["donut"].transform = { {0,-0.75f,-2} ,{0,0,0},{0.5f,0.5f,0.5f} };
		gameObjects["donut"].setShader(&ShaderManager::getShader(ShaderManager::basic), ShaderManager::basic);
		gameObjects["donut"].cull();

		gameObjects["ground"] = Model("prefab/ground.obj");
		gameObjects["ground"].transform = { {0,-1,0},{0,0,0},{10,1,10} };
		gameObjects["ground"].setShader(&ShaderManager::getShader(ShaderManager::basic), ShaderManager::basic);
		//addStatic(physx::PxTransform(0.0f, -1.0f, 0.0f, physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f))), physx::PxPlaneGeometry());
		//addStatic(physx::PxTransform(0.0f, -1.0f, 0.0f), physx::PxBoxGeometry(PxVec3(100.0f,0.5f,100.0f)));

		transparentGameObjects["window"] = Model("prefab/transWindow.obj");
		transparentGameObjects["window"].transform = { {0,-0.02f,2},{0,90,0},{1,1,1} };
		transparentGameObjects["window"].setShader(&ShaderManager::getShader(ShaderManager::basic), ShaderManager::basic);
	}

}