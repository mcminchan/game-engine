#version 330 core

in vec3 normal;
in vec3 fragPos;
out vec4 FragColor;

struct Material
{
	samplerCube texture_diffuse0;
};

uniform Material material;
uniform vec3 cameraPosition;

void main()
{
	vec3 reflectedVector=reflect(normalize(fragPos-cameraPosition),normalize(normal));
	FragColor=texture(material.texture_diffuse0,reflectedVector);
}