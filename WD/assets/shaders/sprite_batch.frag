#version 450 core

struct VertexOutput
{
	vec2 textureCoord;
	float textureID, alpha;
};

layout(location = 0) in VertexOutput f_In;

layout(location = 0) out vec4 o_Color;

layout(binding = 0) uniform sampler2D _Textures[16];

void main()
{
	vec4 color = vec4(f_In.textureCoord, 0.0f, 1.0f);

	int texID = int(f_In.textureID);
	switch(texID)
	{
		case  0: color = texture(_Textures[ 0], f_In.textureCoord); break;
		case  1: color = texture(_Textures[ 1], f_In.textureCoord); break;
		case  2: color = texture(_Textures[ 2], f_In.textureCoord); break;
		case  3: color = texture(_Textures[ 3], f_In.textureCoord); break;
		case  4: color = texture(_Textures[ 4], f_In.textureCoord); break;
		case  5: color = texture(_Textures[ 5], f_In.textureCoord); break;
		case  6: color = texture(_Textures[ 6], f_In.textureCoord); break;
		case  7: color = texture(_Textures[ 7], f_In.textureCoord); break;
		case  8: color = texture(_Textures[ 8], f_In.textureCoord); break;
		case  9: color = texture(_Textures[ 9], f_In.textureCoord); break;
		case 10: color = texture(_Textures[10], f_In.textureCoord); break;
		case 11: color = texture(_Textures[11], f_In.textureCoord); break;
		case 12: color = texture(_Textures[12], f_In.textureCoord); break;
		case 13: color = texture(_Textures[13], f_In.textureCoord); break;
		case 14: color = texture(_Textures[14], f_In.textureCoord); break;
		case 15: color = texture(_Textures[15], f_In.textureCoord); break;
		default: color = vec4(1.0f, 0.0f, 1.0f, 1.0f); break;
	}
	
	if (color.a < 1.0f)
		discard;

	// cheap alpha, asssumes black background and nothing behind
	o_Color = vec4(color.rgb * f_In.alpha, 1.0f);
}
