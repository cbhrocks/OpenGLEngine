#version 330

uniform sampler2D texId;

layout (std140) uniform Scene
{
	float time;
	float gamma;
};

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    FragColor = texture(texId, TexCoords);
	float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
	FragColor = vec4(average, average, average, 1.0);
}
