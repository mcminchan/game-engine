#include "ShaderManager.h"
#include "Shader.h"

namespace ShaderManager
{

	const unsigned size = 5;
	const char* nameList[] = { "basic", "light", "frame", "skybox", "normalTest" };

	static Shader shaders[size];

	void initShaders()
	{
		shaders[basic] = Shader{ "ShaderSources/objectShaderV.vs","ShaderSources/objectShaderV.fs" };
		shaders[light] = Shader{ "ShaderSources/lightShader.vs","ShaderSources/lightShader.fs" };
		shaders[frame] = Shader{ "ShaderSources/frameShader.vs","ShaderSources/frameShader.fs" };
		shaders[skybox] = Shader{ "ShaderSources/skyboxShader.vs", "ShaderSources/skyboxShader.fs" };
		shaders[normalTest] = Shader{ "ShaderSources/normalTest.vs","ShaderSources/normalTest.gs" ,"ShaderSources/normalTest.fs" };
	};

	Shader& getShader(Name name)
	{
		return shaders[name];
	};


}