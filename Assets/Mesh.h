#ifndef PROJECT1_MESH_H
#define PROJECT1_MESH_H

#include "Shader.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};
struct Texture
{
	unsigned int id;
	std::string type; // diffuse, specular 
	std::string path;
};
class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	unsigned _int64 indicesSize;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	Mesh(unsigned int VAO, unsigned _int64 indicesSize, std::vector<Texture> textures);
	Mesh(unsigned int VAO, unsigned _int64 indicesSize);
	//Mesh(Mesh& mesh);

	unsigned _int64 getIndicesSize() const;
	void draw(const Shader& shader) const;
	void setDiffuse(Texture texture);
	unsigned int getVAO() const;


private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};


#endif