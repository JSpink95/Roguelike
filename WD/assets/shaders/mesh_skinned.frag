#version 450

struct FragmentInput
{
	vec3 Normal;
	vec2 UV;
};

layout(location = 0) in FragmentInput VertexOutput;

layout(location = 0) out vec4 o_Color;

const vec3 _LightLocation = vec3(5.0f, 10.0f, 4.0f);

void main()
{
	vec3 lightDirection = normalize(_LightLocation);
	float NL = max(dot(lightDirection, VertexOutput.Normal), 0.1f);
	o_Color = vec4(vec3(1.0f, 0.95f, 0.8f) * NL, 1.0f);
}