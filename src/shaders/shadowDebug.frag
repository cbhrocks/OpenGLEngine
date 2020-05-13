#version 330 core

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

in vec2 TexCoords;

uniform sampler2D depthMap;

out vec4 FragColor;

// required when using a perspective projection matrix
void main()
{             
    float depthValue = texture(depthMap, TexCoords).r;
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}