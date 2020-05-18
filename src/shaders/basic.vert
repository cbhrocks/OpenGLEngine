#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

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

out vec2 TexCoords;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = projection*view*Model*vec4(aPos, 1.0);
}
