#version 330 core

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

uniform mat4 M;  //model matrix
uniform mat3 N;  //normal matrix

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
	gl_Position = projection*view*M*vec4(aPos, 1.0);
    FragPos = vec3(M * vec4(aPos, 1.0));
    Normal = N * aNormal;
}
