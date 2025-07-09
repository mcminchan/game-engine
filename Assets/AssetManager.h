#ifndef PROJECT1_ASSETMANAGER_H
#define PROJECT1_ASSETMANAGER_H

#include "Shader.h"
#include "Model.h"
#include <vector>
#include <map>
#include <string>

namespace AssetManager
{
	extern std::map<std::string, Model> gameObjects;
	extern std::map<std::string, Model> transparentGameObjects;
	extern Model frame;
	extern Model skybox;

	void initGameObjects();
}

#endif