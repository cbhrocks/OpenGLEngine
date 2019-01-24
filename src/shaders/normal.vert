#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 P;
uniform mat4 M;
uniform mat4 C;

out VS_OUT {
    vec3 norm;
} vs_out;

void main()
{
	gl_Position = P*M*C*vec4(pos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(C*M)));
    vs_out.norm = normalize(vec3(P* vec4(normalMatrix * normal, 0.0)));
}
