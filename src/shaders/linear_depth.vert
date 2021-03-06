#version 330

layout (std140) uniform Scene
{
	mat4 projection;
    vec2 window_size;
	float time;
	bool gamma;
	float exposure;
	bool bloom;
};

layout (std140) uniform Camera
{
	mat4 view;
	vec3 camPos;
};
uniform mat4 Model;  //modelview matrix: M = C// * mR * mT

layout (location = 0) in vec3 aPos;
in vec3 colorIn;

smooth out vec3 smoothPos;
smooth out vec3 smoothNorm;
smooth out vec4 smoothColor;
smooth out float distance;

void main()
{
	//smoothPos = pos;
	//smoothNorm = colorIn*2 - 1;
	float d = length(aPos - camPos.xyz);
	gl_Position = projection*view*Model*vec4(aPos, 1);

	//smoothColor = vec4(1,0,0,0);
	distance = d;
}

