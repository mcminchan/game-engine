/*
* ��������� ��ǥ: ���� �ڵ� ������ ���� ��������� 2�� include�ϴ� ���� ���� ������ ������ �����Ѵ�.
* �׷� �� ���� �� ������ ���������� ��� ��������� �̹� include�� ��������� include�ϸ� �߻��Ѵ�.
* �ٵ� ������ ���� ���� ������ ����� �ص� ��� ���� ���ǰ� ���Ե� ���������
* ���� �ٸ� �ڵ忡�� include�Ͽ� ��ũ ������ �߻��ϴ� ���� ���� ���Ѵ�.
* �׷� ������ ������Ͽ� ���Ǹ� �� ���� ���µ� ���Ϸ� ������带 ���°ɱ�??
*
* Ŭ���������̴�. �Լ��� ���� �ڵ忡 �������� �ʰ� ���� �� �� �ְ� 2�� �̻� �����ص� ������ �߻����� �ʴ´�
* �ٵ� Ŭ�������� Ÿ���� ���, ������ �Ұ����ϰ� ���Ǹ� �����ϹǷ�(�޼ҵ� �ڵ� �ۼ��ϴ°� �޼ҵ��� �������� Ŭ������ ���ǰ� �ƴϴ�)
* ���� �ڵ忡 ���� Ŭ���� �̸��� �����ϸ� ������ ������ �߻��Ѵ�.
* �׷� Ŭ���� ��������� ���� �ٸ� �ڵ忡�� include�� �Ұ����Ѱ�?
* -> Ÿ���� ��� ���� ���Ƿ� ���� ��ũ ������ ���ܷ� �߻����� �ʴ´�. �ٸ� �ҽ��ڵ忡 ���� Ÿ���� �־ �������(enum struct class, �ñ��ϸ� �غ���)
* -> �׷��� �������� ���� ���Ƿ� ���� ������ ������ �������ָ� �ȴ�.
* �׷��� ������ ������Ͽ� ���Ǹ� �ϴ°� ��ũ �������� ������ Ŭ������ �����ϰ� ����
* ������ ������ �������� �����ϹǷ� ��� ������ ������ �� �ִ�.
*
* pragma once��� ���尡 ���� �ֱ� �ѵ� �����Ϸ����� �������� �ٸ��� ���Ĵٵ尡 �ƴ϶� ��� �����Ϸ�����
* �۵��� ���Ҽ��� �ִ�. �׸��� VS�� ��쵵 path�� �������� �����ϱ⶧����
* ���纻�� �ٸ� ��ο� �ΰ� include�ϸ� ���� ���Ѵ�.
*/
#ifndef PROJECT1_SHADER_H
#define PROJECT1_SHADER_H

#include <glm/glm.hpp>
#include <string>
#include <Physx/PxPhysicsAPI.h>

class Shader
{
private:
	unsigned int programID;


	unsigned int programShader(const char* source, const char* path, const int type);
	std::string getShaderSource(const char* path);

public:

	Shader(const char* vPath, const char* gPath, const char* fPath);
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader();
	void use() const;
	void setUniform1f(const char* uName, float value) const;
	void setUniform1i(const char* uName, int value) const;
	void setUniformMat4(const char* uName, const glm::mat4& mat) const;
	void setUniformMat4(const char* uName, const physx::PxMat44& mat) const;
	void setUniformMat3(const char* uName, const glm::mat3& mat) const;
	void setUniformMat3(const char* uName, const physx::PxMat33& mat) const;
	void setUniformVec3(const char* uName, const glm::vec3& vec3) const;
	void setUniformBlockPoint(const char* uName, const unsigned int point) const;
	void setSpotLight(std::string uName, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float quadratic, float linear, float constant, float innerCutoff, float outerCutoff) const;
	void setSpotLight(std::string uName, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float quadratic, float linear, float constant, float innerCutoff, float outerCutoff) const;
	void setPointLight(std::string uName, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float quadratic, float linear, float constant) const;
	void setDirectionalLight(std::string uName, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) const;
};



#endif