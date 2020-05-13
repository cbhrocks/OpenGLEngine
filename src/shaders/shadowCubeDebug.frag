#version 330 core

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

in vec2 TexCoords;
in vec3 FragPos;

uniform vec3 lightPos;
uniform float shadowFar;
uniform samplerCube depthMap;

out vec4 FragColor;

// required when using a perspective projection matrix
void main()
{             
	vec3 fragToLight = FragPos - lightPos;
	// closest depth of any object retrieved from the shadowmap
	float closestDepth = texture(depthMap, fragToLight).r;

    FragColor = vec4(vec3(closestDepth - shadowFar), 1.0); // orthographic
}
