#version 450 core

struct VertexOutput
{
    vec3 normal;
    vec4 color;
};

layout(location = 0) in VertexOutput v_Out;
layout(location = 0) out vec4 o_Color;

const vec3 _LightPosition = vec3(28.0f, 16.0f, 32.0f);
const vec3 _LightDirection = normalize(_LightPosition);

void main()
{
    float nl = max(0.2f, dot(_LightDirection, v_Out.normal));

    o_Color = v_Out.color * nl;
}