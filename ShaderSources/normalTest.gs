#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices=6) out;

in mat4 projectionMat[];
in NORM_OUT
{
	vec3 normal;
}norm_in[];

float magnitude=0.1f;
void main()
{
	for(int a=0;a<3;a++)
	{
		gl_Position=projectionMat[0]*(gl_in[a].gl_Position);
		EmitVertex();

		gl_Position=projectionMat[0]*(gl_in[a].gl_Position+vec4(norm_in[a].normal,0.0f)*magnitude);
		EmitVertex();
		EndPrimitive();
	}
}