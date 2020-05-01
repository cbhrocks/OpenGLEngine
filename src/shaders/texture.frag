#version 330

struct Material {
    sampler2D diffuse;
    sampler2D specular;
	sampler2D normal;
	sampler2D height;
};

uniform Material material;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
	FragColor = texture(material.diffuse, TexCoords);
}
