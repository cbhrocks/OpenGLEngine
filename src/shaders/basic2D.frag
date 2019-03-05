#version 330

uniform sampler2D texId;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    FragColor = texture(texId, TexCoords);
}
