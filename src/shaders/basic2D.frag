#version 330

uniform sampler2D texId;

layout (std140) uniform Scene
{
	float time;
	bool gamma;
};

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec3 texColor = texture(texId, TexCoords).rgb;
	if (gamma)
		texColor = pow(texColor, vec3(1.0/2.2));
    FragColor = vec4(texColor, 1.0);
}
