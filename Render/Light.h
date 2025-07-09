#ifndef PROJECT1_LIGHT_H
#define PROJECT1_LIGHT_H
#include "Shader.h"
#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
namespace Light
{
	struct DirectionalLight
	{
		glm::vec3 direction;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 ambient;
		glm::vec3 up;
	};
	struct PointLight  //80
	{
		glm::vec3 position;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 ambient; //48

		float constant; //60
		float linear;  //64
		float quadratic;  //68
	};

	struct SpotLight	//96
	{
		glm::vec3 position;		//0
		glm::vec3 direction;	//16
		glm::vec3 ambient;	//32
		glm::vec3 diffuse;	//48
		glm::vec3 specular;	//64  

		glm::vec3 up;
		glm::vec3 offset;

		float constant;   //76
		float quadratic;  //80 
		float linear;     //84
		float innerCutoff;  //88
		float outerCutoff;  //92
	};

	enum LightType
	{
		pointLight,
		spotLight,
		directionalLight,
		none,
	};

	void setupUniformBuffer();
	void updateLightPosition(unsigned int index, LightType type, const physx::PxVec3& position);
	void updateLightRotation(unsigned int index, const physx::PxVec3 rotation);
	void updateLight(const SpotLight& light, unsigned int index);
	void updateLight(const PointLight& pointL, unsigned int index);
	void updateLight(const DirectionalLight& light);
	void updateLightTex();

	unsigned int addLight(const PointLight& light);
	unsigned int addLight(const SpotLight& light);
	void removeLight(unsigned int index, LightType type);

	bool prepareShadowRendering();

	unsigned int getDirectionalShadowTex();
	const Shader& getShadowShader();
	const DirectionalLight& getDirectionalLight();
	const SpotLight& getSpotLight(unsigned int index);
	const PointLight& getPointLight(unsigned int index);
	const glm::mat4& getLightTransform();
}

#endif