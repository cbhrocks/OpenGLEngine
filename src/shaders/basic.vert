#version 330 core

uniform mat4 P;  //projection matrix
uniform mat4 V;  //camera matrix
uniform mat4 M;  //model matrix

layout (location = 0) in vec3 aPos;

out vec4 vertexColor;

void main()
{
	gl_Position = P*V*M*vec4(aPos, 1.0);
    vertexColor = vec4(0.5, 0.5, 0.5, 1.0);
}

