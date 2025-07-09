#include "Shader.h"
#include "ShaderManager.h"
#include "WindowConst.h"
#include "Buffers.h"
#include "Camera.h"
#include "AssetManager.h"
#include "Light.h"
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <PxPhysicsAPI.h>

enum direction { LEFT, FORWARD, RIGHT, BACK, UP, DOWN };

namespace Camera
{
	static float lastX{ 0 };
	static float lastY{ 0 };
	static float mouseSensitivity{ 0.1f };

	static float fov{ 45.0f };
	static float near{ 0.1f };
	static float far{ 100.0f };
	static bool first{ true };
	static unsigned int cameraUniformBuffer{ 0 };

	extern glm::vec3 cameraPosition{ 0,0,3 };
	extern glm::vec3 cameraDirection{ 0,0,-1 };
	extern glm::vec3 cameraUp{ 0,1,0 };



	extern bool movable{ false };

	extern float speed{ 2 };
	extern float yaw{ -90.0f };
	extern float pitch{ 0 };

	static glm::mat4 lookAt(const glm::vec3& cameraPosition, const glm::vec3& target, const glm::vec3& up)
	{
		glm::mat4 trans{ 1.0f };
		trans = glm::translate(trans, -cameraPosition);
		glm::vec3 direction = cameraPosition - target;	//direction은 +z방향이 기준이다.
		glm::vec3 right = glm::normalize(glm::cross(up, direction));

		/*
		opengl에서 matrix는 column major order이다.
		아래의 rotation을 초기화 하면 right vector가 첫번째 칼럼, up이 두번째 direction이 세번째 칼럼에 위치하게 된다.
		이는 카메라의 rotation을 만드는 matrix이므로 transpose해서 global space의 object들을 카메라의 rotation과 반대로
		rotate하도록 만들어줘야한다.
		*/
		glm::mat4 rotation{ glm::vec4(right,0),glm::vec4(up,0),glm::vec4(direction,0),glm::vec4(0,0,0,1) };
		rotation = glm::transpose(rotation);

		return rotation * trans;
	}

	void setup()
	{

		cameraUniformBuffer = Buffers::genCameraUniformBuffer();
		Buffers::setUniformBufferPoint(cameraUniformBuffer, 0, sizeof(glm::mat4) * 2 + 12);
		for (int a = 0; a < ShaderManager::size; a++)
		{
			ShaderManager::getShader((ShaderManager::Name)a).setUniformBlockPoint("Matrices", 0);
		}
		glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(Camera::getProjection()));
	}
	void setCameraUniformBuffer()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(getViewMat()));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, 12, glm::value_ptr(cameraPosition));
	}

	void setCameraUniformBuffer(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(getViewMat(position, direction, up)));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, 12, glm::value_ptr(position));
	}

	void setModelMat(const Shader& shader, const glm::mat4& model)
	{
		shader.setUniformMat4("model", model);
		shader.setUniformMat3("normMat", glm::mat3(glm::transpose(glm::inverse(model))));
	};

	void setModelMat(const Shader& shader, const physx::PxMat44& model)
	{
		shader.setUniformMat4("model", model);
		const physx::PxMat44& temp = (model.inverseRT()).getTranspose();
		shader.setUniformMat3("normMat", physx::PxMat33(temp.column0.getXYZ(), temp.column1.getXYZ(), temp.column2.getXYZ()));
	};

	void cameraRotate(double xPos, double yPos)
	{
		if (movable == false)
		{
			lastX = xPos;
			lastY = yPos;
			return;
		}
		if (first)
		{
			first = false;
			lastX = xPos;
			lastY = yPos;
		}
		float yawAdded = mouseSensitivity * (xPos - lastX);
		float pitchAdded = mouseSensitivity * (lastY - yPos); 
		lastX = xPos;
		lastY = yPos;

		yaw += yawAdded;
		pitch += pitchAdded;

		if (pitch > 89)
			pitch = 89;
		else if (pitch < -89)
			pitch = -89;
		cameraDirection.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		cameraDirection.y = sin(glm::radians(pitch));
		cameraDirection.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		cameraUp.x = sin(glm::radians(pitch)) * cos(glm::radians(yaw + 180));
		cameraUp.y = cos(glm::radians(pitch));
		cameraUp.z = sin(glm::radians(pitch)) * sin(glm::radians(yaw + 180));
	}
	void zoom(double xScroll, double yScroll)
	{
		fov -= yScroll;
		if (fov > 45)
			fov = 45;
		else if (fov < 1)
			fov = 1;
	}

	void cameraMove(int direction, float deltaTime)
	{
		if (movable == false)
			return;

		glm::vec3 directionVector;
		switch (direction)
		{
		case direction::LEFT:		//left
			directionVector = glm::normalize(glm::cross(cameraUp, cameraDirection)) * deltaTime * speed;
			break;
		case direction::RIGHT:		//left
			directionVector = glm::normalize(glm::cross(cameraDirection, cameraUp)) * deltaTime * speed;
			break;
		case direction::FORWARD:		//left
			directionVector = cameraDirection * deltaTime * speed;
			break;
		case direction::BACK:		//left
			directionVector = -cameraDirection * deltaTime * speed;
			break;
		case direction::UP:		//left
			directionVector = cameraUp * deltaTime * speed;
			break;
		case direction::DOWN:		//left
			directionVector = -cameraUp * deltaTime * speed;
		}
		cameraPosition += directionVector;
	}

	const glm::mat4 getProjection()
	{
		return glm::perspective(glm::radians(fov), (float)Window::renderWidth / Window::renderHeight, near, far);
	}

	const glm::mat4 getViewMat()
	{
		return lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
	}

	const glm::mat4 getViewMat(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up)
	{
		return lookAt(position, position + direction, up);
	}

	glm::vec3 getDirection(const glm::vec3& rotation)
	{

		return glm::angleAxis(glm::radians(rotation.z), glm::vec3{ 0,0,1 }) *
			glm::angleAxis(glm::radians(rotation.y), glm::vec3{ 0,1,0 }) *
			glm::angleAxis(glm::radians(rotation.x), glm::vec3{ 1,0,0 }) * glm::vec3{ 1,0,0 };
	}
	glm::vec3 getUp(const glm::vec3& rotation)
	{
		return glm::angleAxis(glm::radians(rotation.z), glm::vec3{ 0,0,1 }) *
			glm::angleAxis(glm::radians(rotation.y), glm::vec3{ 0,1,0 }) *
			glm::angleAxis(glm::radians(rotation.x), glm::vec3{ 1,0,0 }) * glm::vec3{ 0,1,0 };
	}

}