#version 330

uniform mat4 P;  //projection matrix
uniform mat4 V;  //camera matrix
uniform mat4 M;  //modelview matrix: M = C// * mR * mT

uniform vec3 camPos;      //camera position

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
	gl_Position = P*V*M*pos;

	//smoothColor = vec4(1,0,0,0);
	distance = d;
}

