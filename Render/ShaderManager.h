#ifndef PROJECT1_SHADERMANAGER_H
#define PROJECT1_SHADERMANAGER_H

#include "Shader.h"

namespace ShaderManager
{
	enum Name
	{
		basic
		, light
		, frame
		, skybox
		, normalTest
	};

	extern const unsigned size;
	extern const char* nameList[];

	void initShaders();
	Shader& getShader(Name name);
}

#endif