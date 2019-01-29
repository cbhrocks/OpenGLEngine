#version 330 core

uniform mat4 P;  //projection matrix
uniform mat4 V;  //camera matrix
uniform mat4 M;  //model matrix

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
	gl_Position = P*V*M*vec4(aPos, 1.0);
    FragPos = vec3(M * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(M))) * aNormal;
}

