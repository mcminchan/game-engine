#version 330 core

in vec3 texCoord;
out vec4 FragColor;

struct Material
{
	samplerCube texture_diffuse0;
};
uniform Material material;
void main()
{
	FragColor=texture(material.texture_diffuse0,texCoord);
}