#ifndef CAMERA_H
#define CAMERA_H
#include "Shader.h"
#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
#include <vector>

extern std::vector<Shader> shaders;
namespace Camera
{
	extern glm::vec3 cameraPosition;
	extern glm::vec3 cameraDirection;
	extern glm::vec3 up;

	extern bool movable;


	extern float yaw;
	extern float pitch;
	extern float speed;

	void setup();
	void setModelMat(const Shader& shader, const glm::mat4& model);
	void setModelMat(const Shader& shader, const physx::PxMat44& model);
	void setCameraUniformBuffer();
	void setCameraUniformBuffer(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up);

	const glm::mat4 getProjection();
	const glm::mat4 getViewMat();
	const glm::mat4 getViewMat(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up);
	glm::vec3 getDirection(const glm::vec3& rotation);
	glm::vec3 getUp(const glm::vec3& rotation);

	void zoom(double xScroll, double yScroll);
	void cameraRotate(double xPos, double yPos);
	void cameraMove(int direction, float deltaTime);

};

#endif
