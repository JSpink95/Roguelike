#version 450 core

struct VertexOutput
{
    vec4 color;
};

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec4 a_Color;

layout(location = 0) out VertexOutput v_Out;

layout(std140, binding = 0, set = 0) uniform PerFrameMatrices
{
    mat4 _ProjectionView;
};

void main()
{
    gl_Position = _ProjectionView * vec4(a_Position, 0.0f, 1.0f);
    v_Out.color = a_Color;
}