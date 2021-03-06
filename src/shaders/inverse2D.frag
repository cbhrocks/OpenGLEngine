#version 330

uniform sampler2D texId;

layout (std140) uniform Scene
{
	mat4 projection;
    vec2 window_size;
	float time;
	bool gamma;
};

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vec3(1.0 - texture(texId, TexCoords)), 1.0);
}
