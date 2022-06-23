#version 450 core

struct VertexOutput
{
    vec4 color;
};

layout(location = 0) in VertexOutput v_Out;
layout(location = 0) out vec4 o_Color;

void main()
{
    o_Color = v_Out.color;
}