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
};						// 144 -> 144

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
	sampler2D height;
	//sampler2D reflect;
};

//object data
uniform Material material;

uniform sampler2D texture_shadow_direction;
uniform samplerCube texture_shadow_cube;

in VS_OUT {
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
	//vec4 FragPosLightSpace;
} fs_in;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

vec3 calculateAmbient(vec3 lightAmbient);
vec3 calculateDiffuse(vec3 lightDirection, vec3 viewDirection, vec3 lightDiffuse);
vec3 calculateSpecular(vec3 lightDirection, vec3 viewDirection, vec3 normal, vec3 lightSpecular);

vec3 calculateBasicLight(BasicLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculateDirectionLight(DirectionLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 viewDir = normalize(camPos - fs_in.FragPos);
    vec3 norm = normalize(fs_in.Normal);

    vec3 color = vec3(0.0f);
    for(int i = 0; i < NR_BASIC_LIGHTS; i++) {
		if (length(blight[i].ambient + blight[i].diffuse + blight[i].specular) > 0){
			color += calculateBasicLight(blight[i], norm, fs_in.FragPos, viewDir);
		}
	}
    for(int i = 0; i < NR_DIRECTION_LIGHTS; i++) {
		if (length(dlight[i].ambient + dlight[i].diffuse + dlight[i].specular) > 0){
			color += calculateDirectionLight(dlight[i], norm, fs_in.FragPos, viewDir);
		}
	}
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
		if (length(plight[i].ambient + plight[i].diffuse + plight[i].specular) > 0){
			color += calculatePointLight(plight[i], norm, fs_in.FragPos, viewDir);
		}
	}
    for(int i = 0; i < NR_SPOT_LIGHTS; i++) {
		if (length(slight[i].ambient + slight[i].diffuse + slight[i].specular) > 0){
			color += calculateSpotLight(slight[i], norm, fs_in.FragPos, viewDir);
		}
	}
    //FragColor = vec4(texture(material.diffuse, fs_in.TexCoords).rgb, 1.0f);
    FragColor = vec4(color, 1.0f);

	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else 
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

vec3 calculateAmbient(vec3 lightAmbient) {
    vec3 ambient = lightAmbient * texture(material.diffuse, fs_in.TexCoords).rgb;
	return ambient;
}

vec3 calculateDiffuse(vec3 lightDirection, vec3 normal, vec3 lightDiffuse){
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse =  lightDiffuse * diff * texture(material.diffuse, fs_in.TexCoords).rgb;
	return diffuse;
}

vec3 calculateSpecular(vec3 lightDirection, vec3 viewDirection, vec3 normal, vec3 lightSpecular) {
    vec3 halfwayDir = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
    vec3 specular = lightSpecular * spec * texture(material.specular, fs_in.TexCoords).rgb;
	return specular;
}

vec3 calculateBasicLight(BasicLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    vec3 ambient = calculateAmbient(light.ambient);
	vec3 diffuse = calculateDiffuse(lightDir, normal, light.diffuse);
	vec3 specular = calculateSpecular(lightDir, viewDir, normal, light.specular);

    vec3 finalColor = ambient + diffuse + specular;
    return finalColor;
}

vec3 calculateDirectionLight(DirectionLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

	vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fs_in.FragPos, 1.0); 
	float shadow = 0;

    vec3 ambient = calculateAmbient(light.ambient);
	vec3 diffuse = calculateDiffuse(lightDir, normal, light.diffuse);
	vec3 specular = calculateSpecular(lightDir, viewDir, normal, light.specular);

    vec3 finalColor = ambient + diffuse + specular;
    return finalColor;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    float dist = length(light.position - fragPos);
    vec3 lightDir = normalize(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (gamma ? dist * dist : dist));

    vec3 ambient = calculateAmbient(light.ambient);
	vec3 diffuse = calculateDiffuse(lightDir, normal, light.diffuse);
	vec3 specular = calculateSpecular(lightDir, viewDir, normal, light.specular);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (gamma ? dist * dist : dist));

    vec3 ambient = calculateAmbient(light.ambient);
	vec3 diffuse = calculateDiffuse(lightDir, normal, light.diffuse);
	vec3 specular = calculateSpecular(lightDir, viewDir, normal, light.specular);

    vec3 finalColor = ambient*attenuation + diffuse*intensity*attenuation + specular*intensity*attenuation;

    return finalColor;
}
