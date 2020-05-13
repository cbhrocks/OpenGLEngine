#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 shadowTransform;
uniform mat4 Model;

void main()
{
    gl_Position = shadowTransform * Model * vec4(aPos, 1.0);
}  