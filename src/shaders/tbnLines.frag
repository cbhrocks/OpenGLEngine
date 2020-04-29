#version 330 core

out vec4 fragColor;

in GS_OUT {
	vec4 color;
} fs_in;

void main()
{
    fragColor = fs_in.color;
}
