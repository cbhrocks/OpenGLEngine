#version 330

uniform sampler2D textureUnit;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec3 texColor = texture(textureUnit, TexCoords).rgb;
    FragColor = vec4(texColor, 1.0);
}
