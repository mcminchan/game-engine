#version 330 core

#define MAX_POINTLIGHT 10
#define MAX_SPOTLIGHT 10

in vec3 fragPosition;
in vec3 norm;
in vec2 texCoord;
in vec3 cameraPosition;
in VS_LIGHTPOSES
{
	vec4 directionalLightPos;
	vec4 spotLightPoses[MAX_SPOTLIGHT];
} vs_lightPoses;
out vec4 FragColor;

struct directionalLight
{
	vec3 direction;
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;
};	//size 16*4=64
struct pointLight
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;
	
	float constant;
	float linear;
	float quadratic;
};	//size 16*5 = 80
struct spotLight //96
{
	vec3 position;	//0
	vec3 direction;	//16
	vec3 diffuse;	//32
	vec3 specular;	//48
	vec3 ambient;//64
	
	

	float constant;  //76
	float linear;   //80
	float quadratic;  //84
	float innerCutoff; //88
	float outerCutoff; //92
}; 
struct ShadowTextures
{
	sampler2D directionalShadowTex;
	sampler2D spotShadowTex[MAX_SPOTLIGHT];
};
layout (std140) uniform Light
{
	directionalLight directionalL;
	pointLight pointL[MAX_POINTLIGHT];
	spotLight spotL[MAX_SPOTLIGHT];
	int nb_pointLight;
	int nb_spotLight;
};
struct Material
{
	sampler2D texture_diffuse0;
	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	sampler2D texture_specular0;
	sampler2D texture_specular1;
	sampler2D texture_specular2;
	float shininess;
};
uniform Material material;
uniform ShadowTextures shadowTextures;
uniform bool texturable;

vec3 calDirectionalLight(directionalLight light, vec3 normal, vec3 viewVec, vec3 objColor, vec3 objSpec, float shininess);
vec3 calPointLight(pointLight light, vec3 normal, vec3 viewVec, vec3 objColor, vec3 objSpec, float shininess);
vec3 calSpotLight(spotLight light, int index, vec3 normal, vec3 viewVec, vec3 objColor, vec3 objSpec, float shininess);
float calShadowVal(vec4 pos, sampler2D shadowTexture, float cosV);

void main()
{
	if(texturable)
	{
		vec4 grass=texture(material.texture_diffuse0,texCoord);
		if(grass.a==0)
			discard;
		vec3 normal=normalize(norm);
		vec3 objColor=vec3(grass);
		vec3 objSpec=vec3(texture(material.texture_specular0,texCoord));
		vec3 viewVec=normalize(cameraPosition-fragPosition);

		vec3 tempColor=calDirectionalLight(directionalL,normal,viewVec,objColor,objSpec,1.0f);

		for(int a=0;a<nb_pointLight;a++)
		{
			tempColor+=calPointLight(pointL[a],normal,viewVec,objColor,objSpec,1.0f);
			
		}
		for(int a=0;a<nb_spotLight;a++)
		{
			tempColor+=calSpotLight(spotL[a],a,normal,viewVec,objColor,objSpec,1.0f);
		}

		FragColor=vec4(tempColor,grass.a);
	}
	else
		FragColor=vec4(0.5f,0.5f,0.5f,1.0f);
}

vec3 calDirectionalLight(directionalLight light, vec3 normal, vec3 viewVec, vec3 objColor, vec3 objSpec, float shininess)
{
	vec3 ambient=light.ambient*objColor;

	float diff=max(dot(normalize(-light.direction),normal),0.0f);
	vec3 diffuse=light.diffuse*diff*objColor;

	vec3 specular;
	if(dot(-light.direction,normal)<0)
		specular=vec3(0,0,0);
	else
	{
		vec3 reflectVec=normalize(reflect(light.direction,normal));
		float spec=pow(max(dot(reflectVec,viewVec),0.0f),shininess);
		specular=light.specular*objSpec*spec;
	}

	float shadow=calShadowVal(vs_lightPoses.directionalLightPos,shadowTextures.directionalShadowTex,diff);

	return (ambient+(diffuse+specular)*(1.0-shadow));
}

vec3 calPointLight(pointLight light, vec3 normal, vec3 viewVec, vec3 objColor, vec3 objSpec, float shininess)
{
	float distance=length(light.position-fragPosition);
	float attenuation=1.0/((light.quadratic*distance*distance) + light.linear*distance + light.constant);

	vec3 ambient=light.ambient*objColor;

	vec3 lightVec=normalize(light.position-fragPosition);
	float diff=max(dot(lightVec,normal),0.0f);
	vec3 diffuse=light.diffuse*diff*objColor;

	vec3 reflectVec=normalize(reflect(-lightVec,normal));
	float spec=pow(max(dot(reflectVec,viewVec),0.0f),shininess);
	vec3 specular=light.specular*spec*objSpec;

	
	
	return (ambient+diffuse+specular)*attenuation;
}

vec3 calSpotLight(spotLight light, int index, vec3 normal, vec3 viewVec, vec3 objColor, vec3 objSpec, float shininess)
{
	float distance=length(light.position-fragPosition);
	float attenuation=1.0f/(light.constant + light.linear*distance + light.quadratic*distance*distance);

	vec3 lightVec=normalize(light.position-fragPosition);
	float theta=dot(lightVec,normalize(-light.direction));
	float epsilon=light.innerCutoff-light.outerCutoff;
	float intensity=clamp((theta-light.outerCutoff)/epsilon,0.0f,1.0f);

	vec3 ambient=light.ambient*objColor;

	
	float diff=max(dot(lightVec,normal),0.0f);
	vec3 diffuse=light.diffuse*diff*objColor;

	vec3 halfwayVec=normalize(lightVec+viewVec);
	float spec=pow(max(dot(halfwayVec,normal),0.0f),shininess);
	vec3 specular=light.specular*spec*objSpec;

	float shadow=calShadowVal(vs_lightPoses.spotLightPoses[index],shadowTextures.spotShadowTex[index],diff);

	return (ambient+(diffuse+specular)*intensity*(1.0-shadow))*attenuation;
}

float calShadowVal(vec4 pos,sampler2D shadowTexture, float cosV)
{
	vec4 lightPos=pos/pos.w;
	lightPos=lightPos*0.5+0.5;

	float bias=max(0.05*(1-cosV),0.005);
	float shadowDepthVal=texture(shadowTexture,lightPos.xy).r;
	float currentDepthVal=lightPos.z;

	return currentDepthVal>shadowDepthVal ? 1.0 : 0.0;
}