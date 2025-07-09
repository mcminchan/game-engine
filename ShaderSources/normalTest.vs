#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNorm;

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 projection;
	vec3 cameraPosition;
};

out NORM_OUT
{
	vec3 normal;
}norm_out;
out mat4 projectionMat;

uniform mat4 model;
uniform mat3 normMat;

void main()
{
	gl_Position=view*model*vec4(aPos,1.0f);
	norm_out.normal=normalize(normMat*aNorm);
	projectionMat=projection;
}