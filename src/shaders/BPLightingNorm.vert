#version 330 core

struct BasicLight {
    vec3 ambient;		// 16	//0
    vec3 diffuse;		// 16	//16
    vec3 specular;		// 16	//32	
    vec3 position;		// 16	//48	
};						// 64	//64

struct PointLight {
    vec3 ambient;		// 16	//0
    vec3 diffuse;		// 16	//16
    vec3 specular;		// 16	//32
    vec3 position;		// 16	//48
    float constant;		// 4	//64
    float linear;		// 4	//68
    float quadratic;	// 4	//72
};						// 76 -> 80

struct SpotLight {
    vec3 ambient;		// 16	//0
    vec3 diffuse;		// 16	//16
    vec3 specular;		// 16	//32
    vec3 position;		// 16	//48
    float constant;		// 4	//64
    float linear;		// 4	//68
    float quadratic;	// 4	//72
    vec3 direction;		// 16	//80
    float cutOff;		// 4	//96
    float outerCutOff;	// 4	//100
};//size -> with pad	// 104 -> 112

struct DirectionLight {
    vec3 ambient;		// 16	//0
    vec3 diffuse;		// 16	//16
    vec3 specular;		// 16	//32	
	vec3 position;		// 16	//48
    vec3 direction;		// 16	//64
	mat4 lightSpaceMatrix;
};						// 80 -> 80

#define NR_BASIC_LIGHTS 1
#define NR_POINT_LIGHTS 1
#define NR_SPOT_LIGHTS 1
#define NR_DIRECTION_LIGHTS 1
layout (std140) uniform Lights
{
	BasicLight blight[NR_BASIC_LIGHTS]; // basic light
	PointLight plight[NR_POINT_LIGHTS]; // point light
	SpotLight slight[NR_SPOT_LIGHTS]; // spot light
	DirectionLight dlight[NR_DIRECTION_LIGHTS]; // directional light
};

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 Model;  //model matrix
uniform mat3 Normal;  //normal matrix

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentCamPos;
	vec3 TangentFragPos;
	vec3 TangentLightPos;
	mat3 TBN;
} vs_out;

void main()
{
    vs_out.FragPos = vec3(Model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;

	// 
	vec3 T = normalize(vec3(Model * vec4(aTangent, 0.0)));
	vec3 N = normalize(vec3(Model * vec4(aNormal, 0.0)));
	vec3 B = normalize(vec3(Model * vec4(aBitangent, 0.0)));

	//orthogonalize the two vectors by pushing T down away from the direction N is going.
	T = normalize(T - N * dot(T, N));

	if (dot(cross(N, T), B) < 0.0f){
		T = -T;
	}

	//tbn matrix used to transform from tangent space to world space
	mat3 TBN = mat3(T, B, N);
	vs_out.TBN = TBN;

	//because its an orthogonal matrix we can use the transpose to get the inverse.
	//this inverse is then used to transform world space to tangent space.
	mat3 TBN_inv = transpose(TBN);

	vs_out.TangentCamPos = TBN_inv * camPos;
	vs_out.TangentFragPos = TBN_inv * vs_out.FragPos;
	vs_out.TangentLightPos = TBN_inv * plight[0].position;

	gl_Position = projection*view*Model*vec4(aPos, 1.0);
}
