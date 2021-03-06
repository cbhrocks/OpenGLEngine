#version 330 core

layout (std140) uniform Scene
{
	mat4 projection;
    vec2 window_size;
	float time;
	bool gamma;
	float exposure;
	bool bloom;
};

layout (std140) uniform Camera
{
	mat4 view;
	vec3 camPos;
};

uniform mat4 Model;  //model matrix
uniform mat3 Normal;  //normal matrix

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
} vs_out;

void main()
{
    vs_out.TexCoords = aTexCoords;
	gl_Position = projection*view*Model*vec4(aPos, 1.0);
    vs_out.FragPos = vec3(Model * vec4(aPos, 1.0));
    vs_out.Normal = Normal * aNormal;
}
