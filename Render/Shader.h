/*
* 헤더가드의 목표: 같은 코드 내에서 같은 헤더파일을 2번 include하는 이중 정의 컴파일 에러를 예방한다.
* 그럴 일 없을 것 같지만 간접적으로 어느 헤더파일이 이미 include한 헤더파일을 include하면 발생한다.
* 근데 선언은 같은 파일 내에서 몇번을 해도 상관 없고 정의가 포함된 헤더파일을
* 서로 다른 코드에서 include하여 링크 에러가 발생하는 것은 막지 못한다.
* 그럼 어차피 헤더파일에 정의를 할 일이 없는데 뭐하러 헤더가드를 쓰는걸까??
*
* 클래스때문이다. 함수는 같은 코드에 정의하지 않고 선언만 할 수 있고 2번 이상 선언해도 에러가 발생하지 않는다
* 근데 클래스같은 타입의 경우, 선언이 불가능하고 정의만 가능하므로(메소드 코드 작성하는건 메소드의 정의이지 클래스의 정의가 아니다)
* 같은 코드에 같은 클래스 이름이 존재하면 컴파일 에러가 발생한다.
* 그럼 클래스 헤더파일은 서로 다른 코드에서 include도 불가능한가?
* -> 타입의 경우 이중 정의로 의한 링크 에러는 예외로 발생하지 않는다. 다른 소스코드에 같은 타입이 있어도 상관없다(enum struct class, 궁금하면 해보자)
* -> 그래서 헤더가드로 이중 정의로 의한 컴파일 에러만 예방해주면 된다.
* 그러면 어차피 헤더파일에 정의를 하는건 링크 에러에서 예외인 클래스를 제외하고 없고
* 컴파일 에러는 헤더가드로 예방하므로 모든 에러를 예방할 수 있다.
*
* pragma once라는 가드가 따로 있긴 한데 컴파일러마다 구현법이 다르고 스탠다드가 아니라 어느 컴파일러에선
* 작동을 안할수도 있다. 그리고 VS의 경우도 path를 기준으로 가드하기때문에
* 복사본을 다른 경로에 두고 include하면 막지 못한다.
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