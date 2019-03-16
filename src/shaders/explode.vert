#version 330 core

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

uniform mat4 M;  //model matrix

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec2 texCoords;
} vs_out;

void main()
{
	vs_out.texCoords = aTexCoords;
	gl_Position = projection*view*M*vec4(aPos, 1.0);
}
