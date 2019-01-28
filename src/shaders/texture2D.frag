#version 330

uniform sampler2D texId;

in vec3 TexCoords;
layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = texture(texId, vec2(TexCoords.x, TexCoords.y));
}
