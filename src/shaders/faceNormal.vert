#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

uniform mat4 M; //model matrix

void main()
{
	gl_Position = projection*view*M*vec4(aPos, 1.0);
}
