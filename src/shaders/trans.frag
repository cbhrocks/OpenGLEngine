#version 330

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
};

uniform Material material;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
	vec4 texColor = texture(material.texture_diffuse1, TexCoords);
	if (texColor.a == 0.0)
	{
		discard;
	}
    FragColor = texColor;
}
