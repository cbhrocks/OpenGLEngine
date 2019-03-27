#version 330

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
};

uniform Material material;

in vec2 TexCoords;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main()
{
	vec4 texColor = texture(material.texture_diffuse1, TexCoords);
	if (texColor.a == 0.0)
	{
		discard;
	}
    FragColor = texColor;

	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
