/*
* paired 헤더파일을 굳이 inlcude해야하나?
* ->헤더파일의 내용을 잘못 정의했을때 에러를 컴파일타임에 캐치 가능하다.
* 컴파일 시간이 오래걸리므로 이게 더 이득임.
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
* 왜 헤더파일에 클래스 선언하고 정의를 cpp파일에서 따로 해야하지?
* 헤더파일에 정의를 해두면 2개 이상의 코드에서 헤더파일을 include했을 때 프로그램 하나가 같은 identifier를
* 2번 정의해서 오류가 발생한다.(링크 단계의 오류이다. 컴파일은 각각의 코드가 별개이므로 오류 발생 X)
* 이건 약과고 사실 제일 큰 문제는 헤더파일에 정의한 객체의 코드를 수정하게 되면
* 그 헤더파일을 include한 모든 코드를 다시 컴파일 해야한다. 근데 정의에 필요한 코드를 따로 관리하면
* 그 코드만 수정하고 그 코드만 컴파일하면 된다.(링커가 알아서 수정된 정의를 찾아줄것이기 때문에 헤더파일에 선언만
* 되어있다면 헤더파일을 include한 코드들은 다시 컴파일 할 필요가 없다)
*
* 헤더파일에도 정의가 되어있으니까 그냥 하나의 코드에서 include해도 오류가 발생하는거 아니냐?
* -> 헤더파일은 컴파일 되지 않고(링크 단계에 관여 X) 헤더파일을 include한 코드의 include부분을
* 그냥 헤더파일의 내용으로 대체만 해줄 뿐이다.헤더파일을 별개의 코드나 프로그램으로 생각하면 안되고
* 헤더파일은 그냥 코드에 여러번 쓰기 귀찮은 선언문들을 미리 써두는 텍스트 파일로 보면 된다.
* 한마디로 그냥 파일이다. 파일은 컴파일 하지 않으니까 텍스트 파일에 뭐 함수가 정의되어 있다고 해도
* 파일을 읽어온 코드가 이중정의로 오류가 발생할 리가 없다.
*
* 각각의 코드에서 꼭 필요한 헤더파일들을 다 include해야하는 이유
* 1. 컴파일을 할 수가 없다 -> 헤더파일 읽는건 그렇게 무거운 작업이 아니다. 그냥 텍스트를 읽어오는 것 뿐이다.
* 또 정의가 아닌 선언만 담당하기에 링크단계에서 오류가 발생하지 않는다.
* 2. 간접적으로 include했다고 해도(include한 헤더파일이 다른 필요한 헤더파일을 include) 그 include한 헤더파일에서
* 업데이트 후 다른 헤더파일을 더이상 include지 않게 된다면 컴파일 오류가 발생하게 된다.
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