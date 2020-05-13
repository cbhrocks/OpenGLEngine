#version 330

struct Material {
    sampler2D diffuse;
    sampler2D specular;
	sampler2D normal;
	sampler2D height;
};

uniform Material material;

in vec2 TexCoords;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main()
{
	vec4 texColor = texture(material.diffuse, TexCoords);
	if (texColor.a == 0.0)
	{
		discard;
	}
    FragColor = texColor;

	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
