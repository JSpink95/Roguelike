#version 450 core

struct VertexOutput
{
	vec4 color;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

layout(location = 0) out VertexOutput v_Out;

layout(set=0, binding=0, std140) uniform PerFrameMatrices
{
	mat4 _ProjectionView;
};

void main()
{
	gl_Position = _ProjectionView * vec4(a_Position, 1.0f);
	v_Out.color = a_Color;
}
