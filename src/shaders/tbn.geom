#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices=18) out;

in VS_OUT {
    vec3 normal;
	vec3 tangent;
	vec3 bitangent;
} gs_in[];

const float NORM_LENGTH = 0.4;

void GenerateNormalLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * NORM_LENGTH;
    EmitVertex();
    EndPrimitive();
}

void GenerateTangentLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].tangent, 0.0) * NORM_LENGTH;
    EmitVertex();
    EndPrimitive();
}

void GenerateBitangentLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].bitangent, 0.0) * NORM_LENGTH;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateNormalLine(0);
    GenerateNormalLine(1);
    GenerateNormalLine(2);
    GenerateTangentLine(0);
    GenerateTangentLine(1);
    GenerateTangentLine(2);
    GenerateBitangentLine(0);
    GenerateBitangentLine(1);
    GenerateBitangentLine(2);
}
