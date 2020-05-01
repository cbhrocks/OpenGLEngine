#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

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

void main()
{             
    // retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // then calculate lighting as usual
    vec3 lighting = Albedo * 0.1; // hard-coded ambient component
    vec3 viewDir = normalize(camPos - FragPos);
    for(int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(plight[i].position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * plight[i].diffuse;
		// specular
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
		vec3 specular = plight[i].specular * spec * Specular;
		// attenuation
		float distance = length(plight[i].position - FragPos);
		float attenuation = 1.0 / (1.0 + plight[i].linear * distance + plight[i].quadratic * distance * distance);

		diffuse *= attenuation;
		specular *= attenuation;
		lighting += diffuse + specular;
    }
    
    FragColor = vec4(lighting, 1.0f);
}