#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <glad/glad.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	:vertices{ vertices }, indices{ indices }, textures{ textures }
{
	indicesSize = indices.size();
	setupMesh();
}

//Mesh::Mesh(Mesh& mesh)
//	:vertices{ mesh.vertices }, indices{ mesh.indices }, textures{ mesh.textures }
//{
//	setupMesh();
//}
Mesh::Mesh(unsigned int VAO, unsigned _int64 indicesSize, std::vector<Texture> textures)
	:textures{ textures }
	, indicesSize{ indicesSize }
	, VAO{ VAO }
	, EBO{ 0 }
	, VBO{ 0 }
{
}
Mesh::Mesh(unsigned int VAO, unsigned _int64 indicesSize)
	:VAO{ VAO }
	, indicesSize{ indicesSize }
	, textures{}
{

}

unsigned int Mesh::getVAO() const
{
	return VAO;
};
unsigned _int64 Mesh::getIndicesSize() const
{
	return indicesSize;
};
void Mesh::setupMesh()
{

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));
	glEnableVertexAttribArray(2);
	//glVertexAttribDivisor(0, 1);

};

void Mesh::setDiffuse(Texture texture)
{
	bool changed = false;
	int a;
	for (a = 0; a < textures.size(); a++)
	{
		if (textures[a].type.compare("texture_diffuse") == 0)
		{
			textures.at(a) = texture;
			changed = true;
		}
	}
	if (!changed)
		textures.push_back(texture);
};

void Mesh::draw(const Shader& shader) const
{
	unsigned int diffuseN = 0;
	unsigned int specularN = 0;
	int a;
	std::string temp{};
	for (a = 0; a < textures.size(); a++)
	{
		if (textures[a].type.compare("texture_diffuse") == 0)
			temp = "material.texture_diffuse" + std::to_string(diffuseN++);
		else
			temp = "material.texture_specular" + std::to_string(specularN++);
		shader.setUniform1i(temp.c_str(), a);
		glActiveTexture(GL_TEXTURE0 + a);
		glBindTexture(GL_TEXTURE_2D, textures[a].id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textures[a].id);
	}
	if (a == 0)
	{
		shader.setUniform1i("texturable", 0);
	}
	else
		shader.setUniform1i("texturable", 1);

	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0, 1);
};