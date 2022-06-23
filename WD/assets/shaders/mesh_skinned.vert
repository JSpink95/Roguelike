#version 450

struct FragmentInput
{
	vec3 Normal;
	vec2 UV;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_Texcoord;
layout(location = 3) in vec4 a_Joints;
layout(location = 4) in vec4 a_Weights;

layout(location = 0) out FragmentInput VertexOutput;

layout(std140, set = 0, binding = 0) uniform PerFrameMatrices
{
	mat4 _ProjectionView;
};

layout(std140, set = 1, binding = 1) uniform PerObjectMatrices
{
	mat4 _Transform;
};

//layout(std140, binding=2) uniform SkinnedMeshMatrices
//{
//	mat4 _Matrices[64];
//};

void main()
{
	gl_Position = _ProjectionView * _Transform * vec4(a_Position.xyz, 1.0f);
	VertexOutput.Normal = a_Normal.xyz;
	VertexOutput.UV = a_Texcoord;
}