#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

vec3 GetNormal();
vec4 GetCenter();

const float NORM_LENGTH = 0.4;

void main() {    
    vec3 normal = GetNormal();
	vec4 center = GetCenter();

    gl_Position = center;
    EmitVertex();
    gl_Position = center + vec4(normal, 0.0) * NORM_LENGTH;
    EmitVertex();
    EndPrimitive();
}  

vec4 GetCenter(){
	vec4 a = gl_in[0].gl_Position;
	vec4 b = gl_in[1].gl_Position;
	vec4 c = gl_in[2].gl_Position;
	return (a + b + c)/3;
}

vec3 GetNormal() { 
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}
