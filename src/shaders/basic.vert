#version 330 core

uniform mat4 P;  //projection matrix
uniform mat4 C;  //camera matrix
uniform mat4 M;  //modelview matrix: M = C * mR * mT
uniform mat3 N;  //inverse transpose of upperleft 3x3 of M
uniform mat4 L;  //light rotation matrix

in vec3 pos;

out vec4 vertexColor;

void main()
{
	gl_Position = P * M * vec4(pos, 1.0);
    vertexColor = vec4(0.5, 0.5, 0.5, 1.0);
}

