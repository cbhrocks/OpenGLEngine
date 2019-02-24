#version 330

uniform sampler2D texId;

in vec3 TexCoords;
out vec4 fragColor;

void main()
{
    fragColor = texture(texId, vec2(TexCoords.x, TexCoords.y));
}
