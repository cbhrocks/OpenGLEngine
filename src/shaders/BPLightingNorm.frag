#version 330 core

struct PointLight {
    float ambient;		// 4	//0
    float diffuse;		// 4	//4
    float specular;		// 4	//8
    float constant;		// 4	//12
    float linear;		// 4	//16
    float quadratic;	// 4	//20
    vec3 color;			// 16	//32	//move starting point to divisible
    vec3 position;		// 16	//48
};						// 64

struct SpotLight {
    vec3 color;			// 16	//0
    vec3 position;		// 16	//16		//move starting point to divisible
    vec3 direction;		// 16	//32
    float ambient;		// 4	//34
    float diffuse;		// 4	//38
    float specular;		// 4	//62
    float constant;		// 4	//64
    float linear;		// 4	//68
    float quadratic;	// 4	//72
    float cutOff;		// 4	//76
    float outerCutOff;	// 4	//80
};						// 80

struct DirectionLight {
    float ambient;		// 4	//0
    float diffuse;		// 4	//4
    float specular;		// 4	//8
    vec3 color;			// 16	//16	//move starting point to divisible
    vec3 direction;		// 16	//32	//move starting point to divisible
};						// 48

#define NR_POINT_LIGHTS 1
#define NR_SPOT_LIGHTS 1
#define NR_DIRECTION_LIGHTS 1
layout (std140) uniform Lights
{
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
    sampler2D texture_diffuse;
    sampler2D texture_specular;
	sampler2D texture_normal;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
	float opacity;
	float reflectivity;
	float refractionIndex;
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

void main()
{
	// obtain normal from normal map in range [0,1]
	// only normalized this for nanosuit. shouldnt have to but for some reason did.
    vec3 normalMap = texture(material.texture_normal, fs_in.TexCoords).rgb; 
    // transform normal vector to range [-1,1]
    vec3 normal = normalize(normalMap * 2.0 - 1.0);  // this normal is in tangent space
   
    // get diffuse color for ambient and diffuse
	vec3 diffuseMap = texture(material.texture_diffuse, fs_in.TexCoords).rgb;
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
	float specMap = texture(material.texture_specular, fs_in.TexCoords).r;
	// final specular
    vec3 specular = plight[0].color * plight[0].specular * specMap * spec;
    //vec3 specular = vec3(0.2) * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
    //FragColor = normalize(vec4(fs_in.TBN*normal, 1.0));
    //FragColor = vec4(specMap, 1.0);
}
