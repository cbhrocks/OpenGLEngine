#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

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

uniform mat4 Model; //model matrix
uniform mat3 Normal; //normal matrix

out VS_OUT {
    vec3 normal;
	vec3 tangent;
	vec3 bitangent;
} vs_out;

void main()
{
	gl_Position = projection*view*Model*vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view*Model)));
    vs_out.normal = normalize(vec3(projection * vec4(normalMatrix * aNormal, 0.0)));
    vs_out.tangent = normalize(vec3(projection * vec4(normalMatrix * aTangent, 0.0)));
    vs_out.bitangent = normalize(vec3(projection * vec4(normalMatrix * aBitangent, 0.0)));
}
