#version 450 core

struct VertexOutput
{
    vec3 normal;
    vec4 color;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;

layout(location = 0) out VertexOutput v_Out;

layout(std140, binding = 0, set = 0) uniform PerFrameMatrices
{
    mat4 _ProjectionView;
};

void main()
{
    gl_Position = _ProjectionView * vec4(a_Position, 1.0f);
    v_Out.normal = a_Normal;
    v_Out.color = a_Color;
}