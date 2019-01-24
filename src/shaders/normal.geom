#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices=6) out;

//uniform float normal_length;

in VS_OUT {
    vec3 norm;
} gs_in[];

const float NORM_LENGTH = 0.4;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position; + vec4(gs_in[index].norm, 0.0) * NORM_LENGTH;
    EmitVertex();
    EndPrimitive();
}


void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}
