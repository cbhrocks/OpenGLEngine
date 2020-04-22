#version 330

uniform sampler2D texId;

layout (std140) uniform Scene
{
	float time;
	float gamma;
	float exposure;
};

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec3 texColor = texture(texId, TexCoords).rgb;
	texColor = pow(texColor, vec3(1.0/gamma));
    FragColor = vec4(texColor, 1.0);
}
