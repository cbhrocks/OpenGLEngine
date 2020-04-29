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

layout (std140) uniform Scene
{
	float time;
	bool gamma;
	float exposure;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
	sampler2D normal;
};

//object data
uniform Material material;

in VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentCamPos;
	vec3 TangentFragPos;
	vec3 TangentLightPos; // point light pos
	mat3 TBN;
} fs_in;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

vec3 calculateAmbient(vec3 lightAmbient);
vec3 calculateDiffuse(vec3 lightDirection, vec3 viewDirection, vec3 lightDiffuse);
vec3 calculateSpecular(vec3 lightDirection, vec3 viewDirection, vec3 normal, vec3 lightSpecular);

vec3 calculateBasicLight(BasicLight light, vec3 normal, vec3 viewDir, vec3 tangentLightPos);
vec3 calculateDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir, vec3 tangentLightPos);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 tangentLightPos);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 tangentLightPos);

void main()
{
	// obtain normal from normal map in range [0,1]
	// only normalized this for nanosuit. shouldnt have to but for some reason did.
    vec3 normal = texture(material.normal, fs_in.TexCoords).rgb; 
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
   
    // get diffuse color for ambient and diffuse
	vec3 diffuseMap = texture(material.diffuse, fs_in.TexCoords).rgb;
    // final ambient
    vec3 ambient = plight[0].ambient * diffuseMap;

    // light diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
	// final diffuse
    vec3 diffuse = plight[0].diffuse * diff * diffuseMap;

    // light specular 
    vec3 viewDir = normalize(fs_in.TangentCamPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	// object specular
	vec3 specMap = texture(material.specular, fs_in.TexCoords).rgb;
	// final specular
    vec3 specular = vec3(plight[0].specular) * specMap * spec;
    //vec3 specular = vec3(0.2) * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
    //FragColor = normalize(vec4(fs_in.TBN*normal, 1.0));
}
