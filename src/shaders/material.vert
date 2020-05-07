#version 330 core

layout (location = 0) in vec3 aPos;

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

uniform mat4 Model;  //model matrix

out vec2 TexCoords;

void main()
{
	gl_Position = projection*view*Model*vec4(aPos, 1.0);
}
