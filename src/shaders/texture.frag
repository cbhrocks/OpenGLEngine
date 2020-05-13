#version 330

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
	sampler2D texture_normal;
	sampler2D texture_height;
};

uniform Material material;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
	FragColor = texture(material.texture_diffuse, TexCoords);
}
