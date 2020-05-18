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
	mat4 view;
	vec3 camPos;
};

layout (std140) uniform Scene
{
	mat4 projection;
    vec2 window_size;
	float time;
	bool gamma;
	float exposure;
	bool bloom;
};

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
	sampler2D texture_normal;
	sampler2D texture_height;
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

vec3 calculateDirectionLight(DirectionLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 viewDir = normalize(camPos - fs_in.FragPos);
    vec3 norm = normalize(fs_in.Normal);

    vec3 color = vec3(0.0f);
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

vec3 calculateAmbient(vec3 lightColor, float lightAmbient) {
    vec3 ambient = lightColor * lightAmbient * material.diffuse.rgb * texture(material.texture_diffuse, fs_in.TexCoords).rgb;
	return ambient;
}

vec3 calculateDiffuse(vec3 lightColor, float lightDiffuse, vec3 lightDirection, vec3 normal){
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse =  lightColor * lightDiffuse * diff * material.diffuse.rgb * texture(material.texture_diffuse, fs_in.TexCoords).rgb;
	return diffuse;
}

vec3 calculateSpecular(vec3 lightColor, float lightSpecular, vec3 lightDirection, vec3 viewDirection, vec3 normal) {
    vec3 halfwayDir = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
    vec3 specular = lightColor * lightSpecular * spec * material.specular.rgb * texture(material.texture_specular, fs_in.TexCoords).rgb;
	return specular;
}

vec3 calculateDirectionLight(DirectionLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    vec3 ambient = calculateAmbient(light.color, light.ambient);
	vec3 diffuse = calculateDiffuse(light.color, light.diffuse, lightDir, normal);
	vec3 specular = calculateSpecular(light.color, light.specular, lightDir, viewDir, normal);

    vec3 finalColor = ambient + diffuse + specular;
    return finalColor;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    float dist = length(light.position - fragPos);
    vec3 lightDir = normalize(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (gamma ? dist * dist : dist));

    vec3 ambient = calculateAmbient(light.color, light.ambient);
	vec3 diffuse = calculateDiffuse(light.color, light.diffuse, lightDir, normal);
	vec3 specular = calculateSpecular(light.color, light.specular, lightDir, viewDir, normal);

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

    vec3 ambient = calculateAmbient(light.color, light.ambient);
	vec3 diffuse = calculateDiffuse(light.color, light.diffuse, lightDir, normal);
	vec3 specular = calculateSpecular(light.color, light.specular, lightDir, viewDir, normal);

    vec3 finalColor = ambient*attenuation + diffuse*intensity*attenuation + specular*intensity*attenuation;

    return finalColor;
}
