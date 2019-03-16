#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};
uniform mat4 M;  //model matrix
uniform mat3 N;  //normal matrix

out VS_OUT {
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
} vs_out;

void main()
{
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPos = vec3(M * vec4(aPos, 1.0));
    vs_out.Normal = N * aNormal;
	gl_Position = projection*view*M*vec4(aPos, 1.0);
}
