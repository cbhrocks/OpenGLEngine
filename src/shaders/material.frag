
#version 330

struct Material {
	vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

uniform Material material;

out vec4 FragColor;

void main()
{
	FragColor = material.ambient;
}
