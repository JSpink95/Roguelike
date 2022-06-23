#version 450

struct FragmentInput
{
    vec3 Color;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

layout(location = 0) out FragmentInput VertexOutput;

void main()
{
    gl_Position = vec4(a_Position, 1.0f);
    VertexOutput.Color = a_Color;
}