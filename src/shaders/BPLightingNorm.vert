#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

uniform mat4 Model;  //model matrix
uniform mat3 Normal;  //normal matrix

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentCamPos;
	vec3 TangentFragPos;
	mat3 TBN;
} vs_out;

void main()
{
    vs_out.FragPos = vec3(Model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;

	vec3 T = normalize(Normal * aTangent);
	vec3 N = normalize(Normal * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	vs_out.TBN = transpose(mat3(T, B, N));
	vs_out.TangentCamPos = vs_out.TBN * camPos;
	vs_out.TangentFragPos = vs_out.TBN * vs_out.FragPos;

	gl_Position = projection*view*Model*vec4(aPos, 1.0);
}
