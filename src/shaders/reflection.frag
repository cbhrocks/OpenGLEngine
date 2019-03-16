#version 330

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
};

uniform Material material;
uniform samplerCube skybox;
uniform float reflection;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
	vec3 I = normalize(FragPos - camPos);
	vec3 R = reflect(I, normalize(Normal));
	//FragColor = texture(texture1, TexCoords);
	FragColor = mix(vec4(texture(skybox, R).rgb, 1.0), texture(material.texture_diffuse1, TexCoords), reflection);
}
