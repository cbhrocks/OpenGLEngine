#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 P; //projection matrix
uniform mat4 V; //view matrix
uniform mat4 M; //model matrix

out VS_OUT {
    vec3 normal;
} vs_out;

void main()
{
	gl_Position = P*V*M*vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(V*M)));
    vs_out.normal = normalize(vec3(P * vec4(normalMatrix * aNormal, 0.0)));
}
