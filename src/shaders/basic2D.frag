#version 330

uniform sampler2D texId;

layout (std140) uniform Scene
{
	mat4 projection;
	vec2 screen_size;
	float time;
	bool gamma;
	float exposure;
};

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec3 texColor = texture(texId, TexCoords).rgb;
	texColor = pow(texColor, vec3(1.0/ (gamma ? 2.2 : 0)));
    FragColor = vec4(texColor, 1.0);
}
