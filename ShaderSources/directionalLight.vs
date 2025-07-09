#version 330 core

layout (location=0) in vec3 aPos;

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 projection;
	vec3 cameraPosition;
};

uniform mat4 model;
void main()
{
	gl_Position=projection*mat4(mat3(view))*model*vec4(aPos,1.0f);
	gl_Position.z=gl_Position.w-1.0f;
}