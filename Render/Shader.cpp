/*
* paired ��������� ���� inlcude�ؾ��ϳ�?
* ->��������� ������ �߸� ���������� ������ ������Ÿ�ӿ� ĳġ �����ϴ�.
* ������ �ð��� �����ɸ��Ƿ� �̰� �� �̵���.
*/
#include "Shader.h"
#include <Physx/PxPhysicsAPI.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/*
* �� ������Ͽ� Ŭ���� �����ϰ� ���Ǹ� cpp���Ͽ��� ���� �ؾ�����?
* ������Ͽ� ���Ǹ� �صθ� 2�� �̻��� �ڵ忡�� ��������� include���� �� ���α׷� �ϳ��� ���� identifier��
* 2�� �����ؼ� ������ �߻��Ѵ�.(��ũ �ܰ��� �����̴�. �������� ������ �ڵ尡 �����̹Ƿ� ���� �߻� X)
* �̰� ����� ��� ���� ū ������ ������Ͽ� ������ ��ü�� �ڵ带 �����ϰ� �Ǹ�
* �� ��������� include�� ��� �ڵ带 �ٽ� ������ �ؾ��Ѵ�. �ٵ� ���ǿ� �ʿ��� �ڵ带 ���� �����ϸ�
* �� �ڵ常 �����ϰ� �� �ڵ常 �������ϸ� �ȴ�.(��Ŀ�� �˾Ƽ� ������ ���Ǹ� ã���ٰ��̱� ������ ������Ͽ� ����
* �Ǿ��ִٸ� ��������� include�� �ڵ���� �ٽ� ������ �� �ʿ䰡 ����)
*
* ������Ͽ��� ���ǰ� �Ǿ������ϱ� �׳� �ϳ��� �ڵ忡�� include�ص� ������ �߻��ϴ°� �ƴϳ�?
* -> ��������� ������ ���� �ʰ�(��ũ �ܰ迡 ���� X) ��������� include�� �ڵ��� include�κ���
* �׳� ��������� �������� ��ü�� ���� ���̴�.��������� ������ �ڵ峪 ���α׷����� �����ϸ� �ȵǰ�
* ��������� �׳� �ڵ忡 ������ ���� ������ ���𹮵��� �̸� ��δ� �ؽ�Ʈ ���Ϸ� ���� �ȴ�.
* �Ѹ���� �׳� �����̴�. ������ ������ ���� �����ϱ� �ؽ�Ʈ ���Ͽ� �� �Լ��� ���ǵǾ� �ִٰ� �ص�
* ������ �о�� �ڵ尡 �������Ƿ� ������ �߻��� ���� ����.
*
* ������ �ڵ忡�� �� �ʿ��� ������ϵ��� �� include�ؾ��ϴ� ����
* 1. �������� �� ���� ���� -> ������� �д°� �׷��� ���ſ� �۾��� �ƴϴ�. �׳� �ؽ�Ʈ�� �о���� �� ���̴�.
* �� ���ǰ� �ƴ� ���� ����ϱ⿡ ��ũ�ܰ迡�� ������ �߻����� �ʴ´�.
* 2. ���������� include�ߴٰ� �ص�(include�� ��������� �ٸ� �ʿ��� ��������� include) �� include�� ������Ͽ���
* ������Ʈ �� �ٸ� ��������� ���̻� include�� �ʰ� �ȴٸ� ������ ������ �߻��ϰ� �ȴ�.
*/

Shader::Shader()
{};

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexSource = getShaderSource(vertexPath);
	std::string fragSource = getShaderSource(fragmentPath);
	const char* fShaderSource = fragSource.c_str();
	const char* vShaderSource = vertexSource.c_str();

	unsigned int vertexShader = programShader(vShaderSource, vertexPath, GL_VERTEX_SHADER);
	unsigned int fragmentShader = programShader(fShaderSource, fragmentPath, GL_FRAGMENT_SHADER);

	programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(0);
};

Shader::Shader(const char* vPath, const char* gPath, const char* fPath)
{
	std::string t_vSoruce = getShaderSource(vPath);
	std::string t_gSoruce = getShaderSource(gPath);
	std::string t_fSoruce = getShaderSource(fPath);
	const char* vSource = t_vSoruce.c_str();
	const char* gSource = t_gSoruce.c_str();
	const char* fSource = t_fSoruce.c_str();

	unsigned int vShader = programShader(vSource, vPath, GL_VERTEX_SHADER);
	unsigned int gShader = programShader(gSource, gPath, GL_GEOMETRY_SHADER);
	unsigned int fShader = programShader(fSource, fPath, GL_FRAGMENT_SHADER);

	programID = glCreateProgram();
	glUseProgram(programID);
	glAttachShader(programID, vShader);
	glAttachShader(programID, gShader);
	glAttachShader(programID, fShader);
	glLinkProgram(programID);

	glDeleteShader(vShader);
	glDeleteShader(gShader);
	glDeleteShader(fShader);
	glUseProgram(0);
};

std::string Shader::getShaderSource(const char* path)
{
	std::string source;
	std::ifstream file;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		file.open(path);

		std::stringstream stream;
		stream << file.rdbuf();

		file.close();
		source = stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << path << ": shader file exception" << std::endl;
	}
	return source;
};

unsigned int Shader::programShader(const char* source, const char* path, const int type)
{
	char logBf[512];
	int success;
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, sizeof(logBf), NULL, logBf);
		std::cout << path << "\n" << logBf << std::endl;
	}
	return shader;
};
void Shader::use() const
{
	glUseProgram(Shader::programID);
};

void Shader::setUniform1f(const char* uName, float value) const
{
	unsigned int location = glGetUniformLocation(Shader::programID, uName);
	glUniform1f(location, value);
};

void Shader::setUniform1i(const char* uName, int value) const
{
	unsigned int location = glGetUniformLocation(Shader::programID, uName);
	glUniform1i(location, value);
};

void Shader::setUniformVec3(const char* uName, const glm::vec3& vec3)const
{
	unsigned int location = glGetUniformLocation(Shader::programID, uName);
	glUniform3fv(location, 1, glm::value_ptr(vec3));
};

void Shader::setUniformMat4(const char* uName, const glm::mat4& mat) const
{
	unsigned int location = glGetUniformLocation(Shader::programID, uName);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
};

void Shader::setUniformMat4(const char* uName, const physx::PxMat44& mat) const
{
	unsigned int location = glGetUniformLocation(Shader::programID, uName);
	glUniformMatrix4fv(location, 1, GL_FALSE, &mat.column0.x);
};

void Shader::setUniformMat3(const char* uName, const glm::mat3& mat) const
{
	unsigned int location = glGetUniformLocation(Shader::programID, uName);
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
};
void Shader::setUniformMat3(const char* uName, const physx::PxMat33& mat) const
{
	unsigned int location = glGetUniformLocation(Shader::programID, uName);
	glUniformMatrix3fv(location, 1, GL_FALSE, &mat.column0.x);
};

void Shader::setUniformBlockPoint(const char* uName, const unsigned int point) const
{
	unsigned int index = glGetUniformBlockIndex(programID, uName);
	glUniformBlockBinding(programID, index, point);
};

void Shader::setSpotLight(std::string uName, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float quadratic, float linear, float constant, float innerCutoff, float outerCutoff) const
{
	setPointLight(uName, position, ambient, diffuse, specular, quadratic, linear, constant);
	setUniformVec3((uName + ".direction").c_str(), direction);
	setUniform1f((uName + ".innerCutoff").c_str(), innerCutoff);
	setUniform1f((uName + ".outerCutoff").c_str(), outerCutoff);
};
void Shader::setSpotLight(std::string uName, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float quadratic, float linear, float constant, float innerCutoff, float outerCutoff) const
{
	setUniformVec3((uName + ".ambient").c_str(), ambient);
	setUniformVec3((uName + ".diffuse").c_str(), diffuse);
	setUniformVec3((uName + ".specular").c_str(), specular);
	setUniform1f((uName + ".quadratic").c_str(), quadratic);
	setUniform1f((uName + ".linear").c_str(), linear);
	setUniform1f((uName + ".constant").c_str(), constant);
	setUniform1f((uName + ".innerCutoff").c_str(), innerCutoff);
	setUniform1f((uName + ".outerCutoff").c_str(), outerCutoff);
};
void Shader::setPointLight(std::string uName, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float quadratic, float linear, float constant) const
{
	setUniformVec3((uName + ".position").c_str(), position);
	setUniformVec3((uName + ".ambient").c_str(), ambient);
	setUniformVec3((uName + ".diffuse").c_str(), diffuse);
	setUniformVec3((uName + ".specular").c_str(), specular);
	setUniform1f((uName + ".quadratic").c_str(), quadratic);
	setUniform1f((uName + ".linear").c_str(), linear);
	setUniform1f((uName + ".constant").c_str(), constant);
};

void Shader::setDirectionalLight(std::string uName, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) const
{
	setUniformVec3((uName + ".direction").c_str(), direction);
	setUniformVec3((uName + ".ambient").c_str(), ambient);
	setUniformVec3((uName + ".diffuse").c_str(), diffuse);
	setUniformVec3((uName + ".specular").c_str(), specular);
};