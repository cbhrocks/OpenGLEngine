#version 330 core

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

in vec2 TexCoords;

uniform sampler2D textureID;
uniform float near;
uniform float far;

out vec4 FragColor;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             
    float depthValue = texture(textureID, TexCoords).r;
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}