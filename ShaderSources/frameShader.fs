#version 330 core

in vec2 texCoord;

out vec4 FragColor;

struct Material
{
	sampler2D texture_diffuse0;
};

uniform Material material;

const float offset=1.0f/300.0f;
void main()
{
	vec2 offsets[9]={{-offset,offset}
					,{0,offset}
					,{offset,offset}
					,{-offset,0}
					,{0,0}
					,{offset,0}
					,{-offset,-offset}
					,{0,-offset}
					,{offset,-offset}};
	float kernel[9]={1/16.0,2/16.0,1/16.0
					,2/16.0,4/16.0,2/16.0
					,1/16.0,2/16.0,1/16.0};
	vec4 color=vec4(0);
	for(int a=0;a<9;a++)
	{
	//	color+=kernel[a]*vec4(texture(texture0,texCoord+offsets[a]));
	}
	float gamma=2.2;
	FragColor=vec4(texture(material.texture_diffuse0,texCoord).rgb,1.0f);
	FragColor.rgb=pow(FragColor.rgb,vec3(1.0/gamma));
}