#version 330

layout (location = 0) in vec3 pos;

out vec3 TexCoords;

void main()
{
    vec3 tp = (pos + vec3(1, 1, 1))/2;
    gl_Position = vec4(pos, 1.0);

    TexCoords = tp;
}
