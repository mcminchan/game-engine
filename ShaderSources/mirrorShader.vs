#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;

out vec3 normal;
out vec3 fragPos;

uniform mat4 model;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
	vec3 cameraPosition;
};
uniform mat3 normMat;

void main()
{
	vec4 temp=model*vec4(aPos,1.0f);
	gl_Position=projection*view*temp;
	fragPos=vec3(temp);
	normal=normMat*aNorm;
}