#version 330 core

uniform mat4 P;  //projection matrix
uniform mat4 C;  //camera matrix
uniform mat4 M;  //modelview matrix: M = C * mR * mT

layout (location = 0) in vec3 pos;

out vec4 vertexColor;

void main()
{
	gl_Position = P*C*M*vec4(pos, 1.0);
    vertexColor = vec4(0.5, 0.5, 0.5, 1.0);
}

