#version 330

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};
uniform mat4 M;  //modelview matrix: M = C// * mR * mT

layout (location = 0) in vec3 pos;
in vec3 colorIn;

smooth out vec3 smoothPos;
smooth out vec3 smoothNorm;
smooth out vec4 smoothColor;
smooth out float distance;

void main()
{
	//smoothPos = pos;
	//smoothNorm = colorIn*2 - 1;
	float d = length(pos - camPos.xyz);
	vec4 pos = vec4(pos, 1);
	gl_Position = projection*view*M*pos;

	//smoothColor = vec4(1,0,0,0);
	distance = d;
}

