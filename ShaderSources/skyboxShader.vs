#version 330 core

layout(location = 0) in vec3 aPos;

out vec3 texCoord;


layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
	vec3 cameraPosition;
};

void main()
{
	texCoord=aPos;
	gl_Position=projection*mat4(mat3(view))*vec4(aPos,1.0f);
	gl_Position.z=gl_Position.w;
}