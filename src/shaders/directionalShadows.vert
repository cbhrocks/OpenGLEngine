#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 Model;
uniform mat3 Normal;
uniform mat4 shadowTransform;

void main()
{
    vs_out.FragPos = vec3(Model * vec4(aPos, 1.0));
    vs_out.Normal = Normal * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = shadowTransform * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * Model * vec4(aPos, 1.0);
}