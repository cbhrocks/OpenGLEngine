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
    vec3 texColor = texture(texId, TexCoords).rgb;

	//reinhard tone mapping
	vec3 mapped = texColor / (texColor + vec3(1.0));

	texColor = pow(texColor, vec3(1.0/ (gamma ? 2.2f : 1.0f)));
    FragColor = vec4(mapped, 1.0);
}
