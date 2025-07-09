#ifndef PROJECT1_TEXTURELOADER_H
#define PROJECT1_TEXTURELOADER_H
#include <vector>
#include <string>

namespace TextureLoader
{
	unsigned int get2DTextureId(const char* filePath);
	unsigned int getSkyboxTexture(std::vector<std::string> facePath);
}

#endif