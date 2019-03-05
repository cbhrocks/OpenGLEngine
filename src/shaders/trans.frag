#version 330

uniform sampler2D texture1;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
	vec4 texColor = texture(texture1, TexCoords);
	if (texColor.a == 0.0)
	{
		discard;
	}
    FragColor = texColor;
}
