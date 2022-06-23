#version 450

struct FragmentInput
{
    vec3 Color;
};

layout(location = 0) in FragmentInput VertexOutput;

layout(location = 0) out vec4 Out_Color;

void main()
{
    Out_Color = vec4(VertexOutput.Color, 1.0f);
}
