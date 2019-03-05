#version 330 core

uniform mat4 P;  //projection matrix
uniform mat4 V;  //camera matrix

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

void main()
{
	TexCoords = aPos;
	gl_Position = (P*V*vec4(aPos, 1.0)).xyww;
}
