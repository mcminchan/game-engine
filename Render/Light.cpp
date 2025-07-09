#include "AssetManager.h"
#include "ShaderManager.h"
#include "Buffers.h"
#include "Camera.h"
#include "Light.h"
#include "Shader.h"
#include "WindowConst.h"
#include <glad/glad.h>
#include <iostream>
#include <Physx/PxPhysicsAPI.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Light
{

	static const int SIZE_DIRECTIONALLIGHT{ 64 };
	static const int SIZE_POINTLIGHT{ 80 };
	static const int SIZE_SPOTLIGHT{ 96 };
	static const int NB_POINTLIGHT{ 10 };
	static const int NB_SPOTLIGHT{ 10 };
	static const int SIZE_FLOAT{ 8 };
	static unsigned int uniformBuffer;

	static unsigned int gDirectionalLightFrameBuffer = 0;
	static std::vector<unsigned int> gSpotLightFrameBuffers;
	static std::vector<unsigned int> gPointFrameBuffers;

	static unsigned int gDirectionalShadowTex;
	static std::vector<unsigned int> gSpotShadowTexs;
	static std::vector<unsigned int> gPointShadowTexs;

	static glm::mat4 gDirectionalTransform;
	static std::vector<glm::mat4> gSpotLightTransforms;

	static glm::mat4 gDirectionalProjection;

	static float gFov{ 45.0f };
	static float gNear{ 0.1f };
	static float gFar{ 20.0f };

	static Shader gShadowShader;


	static DirectionalLight directionalL;
	static std::vector<PointLight> pointLights;
	static std::vector<SpotLight> spotLights;
	static Shader directionalLightShader;

	static void updateSpotLightTransform(unsigned int index);
	static void updateDirectionalLightTransform();

	static void createDirectionalLightFrameBuffer();
	static void createSpotLightFrameBuffer();

	void setupUniformBuffer()
	{
		unsigned int size = SIZE_DIRECTIONALLIGHT
			+ SIZE_POINTLIGHT * NB_POINTLIGHT
			+ SIZE_SPOTLIGHT * NB_SPOTLIGHT + SIZE_FLOAT;
		glGenBuffers(1, &uniformBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
		Buffers::setUniformBufferPoint(uniformBuffer, 1, size);
		for (int a = 0; a < ShaderManager::size; a++)
			ShaderManager::getShader((ShaderManager::Name)a).setUniformBlockPoint("Light", 1);

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		directionalLightShader = Shader{ "ShaderSources/directionalLight.vs","ShaderSources/lightShader.fs" };
		gShadowShader = Shader{ "ShaderSources/shadowShader.vs","ShaderSources/shadowShader.fs" };
		gDirectionalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, gNear, gFar);
	}

	void updateLightPosition(unsigned int index, LightType type, const physx::PxVec3& position)//or direction of dLight
	{
		int offset;
		if (type == pointLight)
		{
			if (index > pointLights.size())
			{
				std::cout << "out of bound" << std::endl;
				return;
			}
			pointLights.at(index).position = glm::vec3(position.x, position.y, position.z);
			offset = SIZE_DIRECTIONALLIGHT + SIZE_POINTLIGHT * index;
		}
		else if (type == spotLight)
		{
			if (index > spotLights.size())
			{
				std::cout << "out of bound(spotLight)" << std::endl;
				return;
			}
			SpotLight& light = spotLights.at(index);

			light.position = glm::vec3(position.x, position.y, position.z);

			updateSpotLightTransform(index);
			offset = SIZE_DIRECTIONALLIGHT + SIZE_POINTLIGHT * NB_POINTLIGHT + index * SIZE_SPOTLIGHT;

		}
		else
		{
			offset = 0;
		}
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(physx::PxVec3), &position);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void updateLightRotation(unsigned int index, const physx::PxVec3 rotation)
	{
		SpotLight& light = spotLights.at(index);
		glm::quat q{ glm::vec3(glm::radians(rotation.x + light.offset.x),
							   glm::radians(rotation.y + light.offset.y),
							   glm::radians(rotation.z + light.offset.z)) };
		light.direction = q * glm::vec3{ 1,0,0 };
		light.up = q * glm::vec3{ 0,1,0 };
		int offset = SIZE_DIRECTIONALLIGHT + SIZE_POINTLIGHT * NB_POINTLIGHT + SIZE_SPOTLIGHT * index + 16;
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &light.direction);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);


		updateSpotLightTransform(index);
	}

	void updateLight(const PointLight& pointL, unsigned int index)
	{
		pointLights.at(index) = pointL;
		int size = pointLights.size() + 1;
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
		unsigned int offset = SIZE_DIRECTIONALLIGHT + SIZE_POINTLIGHT * index;

		glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, (void*)&(pointL.position));
		offset += 16;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, (void*)&(pointL.diffuse));
		offset += 16;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, (void*)&(pointL.specular));
		offset += 16;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, (void*)&(pointL.ambient));
		offset += 12;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, 4, (void*)&(pointL.constant));
		offset += 4;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, 4, (void*)&(pointL.linear));
		offset += 4;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, 4, (void*)&(pointL.quadratic));

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

	}

	void updateLight(const SpotLight& light, unsigned int index)
	{
		spotLights.at(index) = light;
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
		int offset = SIZE_DIRECTIONALLIGHT + SIZE_POINTLIGHT * NB_POINTLIGHT + SIZE_SPOTLIGHT * index;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &light.position);
		offset += 16;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &light.direction);
		offset += 16;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &light.diffuse);
		offset += 16;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &light.specular);
		offset += 16;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &light.ambient);
		offset += 12;

		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &light.constant);
		offset += 4;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &light.linear);
		offset += 4;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &light.quadratic);
		offset += 4;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &light.innerCutoff);
		offset += 4;
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &light.outerCutoff);

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		updateSpotLightTransform(index);
	}

	void updateLight(const DirectionalLight& light)
	{
		directionalL = light;
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, (void*)&light.direction);
		glBufferSubData(GL_UNIFORM_BUFFER, 16, 12, (void*)&light.diffuse);
		glBufferSubData(GL_UNIFORM_BUFFER, 32, 12, (void*)&light.specular);
		glBufferSubData(GL_UNIFORM_BUFFER, 48, 12, (void*)&light.ambient);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		if (gDirectionalLightFrameBuffer == 0)
			createDirectionalLightFrameBuffer();
		updateDirectionalLightTransform();
	}

	static void updateSpotLightTransform(unsigned int index)
	{
		const Shader& shader = ShaderManager::getShader(ShaderManager::basic);
		shader.use();

		const SpotLight& light = spotLights.at(index);
		glm::mat4 view = glm::lookAt(light.position, light.position + light.direction, light.up);
		float fov = glm::radians(glm::degrees(acos(light.outerCutoff)) * 2);
		glm::mat4 projection = glm::perspective(fov, 1.0f, gNear, gFar);
		if (gSpotLightTransforms.size() < spotLights.size())
		{
			gSpotLightTransforms.push_back(projection * view);
			shader.setUniform1i("lightTransforms.nb_spotLights", gSpotLightTransforms.size());
		}
		else
			gSpotLightTransforms.at(index) = projection * view;

		std::string temp = "lightTransforms.spotLightTransforms[" + std::to_string(index) + "]";
		shader.setUniformMat4(temp.c_str(), gSpotLightTransforms.at(index));
	}
	static void updateDirectionalLightTransform()
	{
		glm::mat4 view = glm::lookAt(-50.0f * directionalL.direction, glm::vec3(0, 0, 0), directionalL.up);
		gDirectionalTransform = gDirectionalProjection * view;

		const Shader& shader = ShaderManager::getShader(ShaderManager::basic);
		shader.use();
		shader.setUniformMat4("lightTransforms.directionalLightTransform", gDirectionalTransform);
	}

	void updateLightTex()
	{
		const Shader& objectShader = ShaderManager::getShader(ShaderManager::basic);
		objectShader.use();
		objectShader.setUniform1i("shadowTextures.directionalShadowTex", 7);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, gDirectionalShadowTex);

		for (int a = 0; a < spotLights.size(); a++)
		{
			std::string temp = "shadowTextures.spotShadowTex[" + std::to_string(a) + "]";
			objectShader.setUniform1i(temp.c_str(), 8 + a);
			glActiveTexture(GL_TEXTURE8 + a);
			glBindTexture(GL_TEXTURE_2D, gSpotShadowTexs[a]);
		}
	}

	unsigned int addLight(const PointLight& light)
	{
		if (pointLights.size() >= NB_POINTLIGHT)
		{
			std::cout << "no more pointlights" << std::endl;
			return -1;
		}
		pointLights.push_back(light);
		int size = pointLights.size();
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, SIZE_DIRECTIONALLIGHT + SIZE_POINTLIGHT * NB_POINTLIGHT
			+ SIZE_SPOTLIGHT * NB_SPOTLIGHT, sizeof(int32_t), &size);
		updateLight(light, size - 1);
		return size - 1;

	}
	unsigned int addLight(const SpotLight& light)
	{
		if (spotLights.size() >= NB_SPOTLIGHT)
		{
			std::cout << "no more pointlights" << std::endl;
			return -1;
		}
		spotLights.push_back(light);
		int size = spotLights.size();
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, SIZE_DIRECTIONALLIGHT + SIZE_POINTLIGHT * NB_POINTLIGHT
			+ SIZE_SPOTLIGHT * NB_SPOTLIGHT + sizeof(int32_t), sizeof(int32_t), &size);
		updateLight(light, size - 1);
		createSpotLightFrameBuffer();

		return size - 1;
	}

	void removeLight(unsigned int index, LightType type)
	{
		if (type == spotLight)
		{
			if (index < 0 || index >= spotLights.size())
			{
				std::cout << "out of bound" << std::endl;
				return;
			}
			glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
			spotLights.erase(spotLights.begin() + index);
			int size = spotLights.size();
			for (int a = index; a < size; a++)
			{
				updateLight(spotLights.at(a), a);
			}
			for (auto a = AssetManager::gameObjects.begin(); a != AssetManager::gameObjects.end(); a++)
			{
				a->second.subOneLightIndex(index);
			}
			glBufferSubData(GL_UNIFORM_BUFFER, SIZE_DIRECTIONALLIGHT + SIZE_POINTLIGHT * NB_POINTLIGHT
				+ SIZE_SPOTLIGHT * NB_SPOTLIGHT + sizeof(int32_t), sizeof(int32_t), &size);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
		else if (type == pointLight)
		{
			if (index < 0 || index >= pointLights.size())
			{
				std::cout << "out of bound" << std::endl;
				return;
			}
			glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
			pointLights.erase(pointLights.begin() + index);
			int size = pointLights.size();
			for (int a = index; a < size; a++)
			{
				updateLight(pointLights.at(a), a);
			}
			for (auto a = AssetManager::gameObjects.begin(); a != AssetManager::gameObjects.end(); a++)
			{
				a->second.subOneLightIndex(index);
			}
			glBufferSubData(GL_UNIFORM_BUFFER, SIZE_DIRECTIONALLIGHT + SIZE_POINTLIGHT * NB_POINTLIGHT
				+ SIZE_SPOTLIGHT * NB_SPOTLIGHT, sizeof(int32_t), &size);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}

	static void createDirectionalLightFrameBuffer()
	{
		glGenFramebuffers(1, &gDirectionalLightFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gDirectionalLightFrameBuffer);

		glGenTextures(1, &gDirectionalShadowTex);
		glBindTexture(GL_TEXTURE_2D, gDirectionalShadowTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Window::renderWidth, Window::renderHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		const float borderColor[] = { 1.0f,1.0f,1.0f,1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDirectionalShadowTex, 0);
		glReadBuffer(GL_NONE);
		glDrawBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	static void createSpotLightFrameBuffer()
	{
		if (gSpotLightFrameBuffers.size() < spotLights.size())
		{
			unsigned int buffer;
			unsigned int texture;
			glGenFramebuffers(1, &buffer);
			glBindFramebuffer(GL_FRAMEBUFFER, buffer);

			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Window::renderWidth, Window::renderHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			const float borderColor[] = { 1.0f,1.0f,1.0f,1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
			glReadBuffer(GL_FALSE);
			glDrawBuffer(GL_FALSE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			gSpotLightFrameBuffers.push_back(buffer);
			gSpotShadowTexs.push_back(texture);
		}
	}

	//return false if there is no shadow framebuffer that has to be rendered
	bool prepareShadowRendering()
	{
		static int index = 0;
		int nb_lights = 1 + spotLights.size();
		if (index >= nb_lights)
		{
			index = 0;
			return false;
		}
		else
		{
			if (index == 0)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, gDirectionalLightFrameBuffer);
				glEnable(GL_DEPTH_TEST);
				glClear(GL_DEPTH_BUFFER_BIT);
				gShadowShader.use();
				gShadowShader.setUniformMat4("lightTransform", gDirectionalTransform);
			}
			else
			{
				glBindFramebuffer(GL_FRAMEBUFFER, gSpotLightFrameBuffers.at(index - 1));
				glEnable(GL_DEPTH_TEST);
				glClear(GL_DEPTH_BUFFER_BIT);
				gShadowShader.setUniformMat4("lightTransform", gSpotLightTransforms.at(index - 1));
			}
		}
		index++;
		return true;
	}

	unsigned int getDirectionalShadowTex()
	{
		if (gSpotShadowTexs.size() > 0)
			return gSpotShadowTexs.at(0);
		else
			gDirectionalShadowTex;
	}
	const Shader& getShadowShader()
	{
		return gShadowShader;
	}
	const DirectionalLight& getDirectionalLight()
	{
		return directionalL;
	}
	const SpotLight& getSpotLight(unsigned int index)
	{
		if (index >= spotLights.size() || index < 0)
		{
			std::cout << "out of bound" << std::endl;
			return SpotLight{};
		}
		return spotLights.at(index);
	}
	const PointLight& getPointLight(unsigned int index)
	{
		if (index >= pointLights.size() || index < 0)
		{
			std::cout << "out of bound" << std::endl;
			return PointLight{};
		}
		return pointLights.at(index);
	}

	const glm::mat4& getLightTransform()
	{
		return gDirectionalTransform;
	}
}