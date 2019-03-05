#version 330

uniform sampler2D texId;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vec3(1.0 - texture(texId, TexCoords)), 1.0);
}
