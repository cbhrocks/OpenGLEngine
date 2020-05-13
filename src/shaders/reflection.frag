#version 330

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
	sampler2D normal;
	sampler2D height;
};

uniform Material material;
uniform samplerCube skybox;
uniform float reflection;

in VS_OUT {
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
} fs_in;

out vec4 FragColor;

void main()
{
	vec3 I = normalize(fs_in.FragPos - camPos);
	vec3 R = reflect(I, normalize(fs_in.Normal));
	//FragColor = texture(texture1, TexCoords);
	FragColor = mix(vec4(texture(skybox, R).rgb, 1.0), texture(material.diffuse, fs_in.TexCoords), reflection);
}
