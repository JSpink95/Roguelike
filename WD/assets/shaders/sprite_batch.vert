#version 450 core

struct VertexOutput
{
	vec2 textureCoord;
	float textureID, alpha;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_Texcoord;
layout(location = 2) in vec2 a_TextureID_Alpha;

layout(location = 0) out VertexOutput v_Out;

layout(std140, set = 0, binding = 0) uniform PerFrameMatrices
{
	mat4 _ProjectionView;
};

layout(std140, set = 1, binding = 1) uniform PerObjectMatrices
{
	mat4 _Transform;
};

void main()
{
	gl_Position = _ProjectionView * vec4(a_Position, 1.0f);

	v_Out.textureCoord = a_Texcoord;
	v_Out.textureID = a_TextureID_Alpha.x;
	v_Out.alpha = a_TextureID_Alpha.y;
}
