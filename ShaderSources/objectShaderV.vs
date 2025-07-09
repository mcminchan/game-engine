#version 330 core
#define MAX_SPOTLIGHTS 10

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexCoord;

layout (std140) uniform Matrices
{
    mat4 view;			//0 -> 64
    mat4 projection;		//64 -> 128
	vec3 cameraPos;			//128 -> 140
};

struct LightTransforms
{
	mat4 directionalLightTransform;
	mat4 spotLightTransforms[MAX_SPOTLIGHTS];
	int nb_spotLights;
};
uniform mat4 model;
uniform mat3 normMat;
uniform LightTransforms lightTransforms;

out VS_LIGHTPOSES
{
	vec4 directionalLightPos;
	vec4 spotLightPoses[MAX_SPOTLIGHTS];
} vs_lightPoses;
out vec3 norm;
out vec3 fragPosition;
out vec2 texCoord;
out vec3 cameraPosition;


void main()
{
	vec4 globalPos=model*vec4(aPos,1.0f);
	fragPosition=vec3(globalPos);
	gl_Position=projection*view*globalPos;
	norm=normMat*aNorm;
	texCoord=aTexCoord;
	cameraPosition=cameraPos;
	
	vs_lightPoses.directionalLightPos=lightTransforms.directionalLightTransform*globalPos;
	for(int a=0;a<lightTransforms.nb_spotLights;a++)
	{
		vs_lightPoses.spotLightPoses[a]=lightTransforms.spotLightTransforms[a]*globalPos;
	}
}