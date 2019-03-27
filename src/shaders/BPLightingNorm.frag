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
	float fake;
    float constant;		// 4	//64
    float linear;		// 4	//68
    float quadratic;	// 4	//72
};						// 76 -> 80

struct SpotLight {
    vec3 ambient;		// 16	//0
    vec3 diffuse;		// 16	//16
    vec3 specular;		// 16	//32
    vec3 position;		// 16	//48
	float fake;
    float constant;		// 4	//64
    float linear;		// 4	//68
    float quadratic;	// 4	//72
    vec3 direction;		// 16	//80
	float fake2;
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
};

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
	sampler2D texture_shadow_direction1;
	samplerCube texture_shadow_cube1;
};

//object data
uniform Material material;

in VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentCamPos;
	vec3 TangentFragPos;
	mat3 TBN;
} fs_in;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

vec3 calculateAmbient(vec3 lightAmbient);
vec3 calculateDiffuse(vec3 lightDirection, vec3 viewDirection, vec3 lightDiffuse);
vec3 calculateSpecular(vec3 lightDirection, vec3 viewDirection, vec3 normal, vec3 lightSpecular);

vec3 calculateBasicLight(BasicLight light, vec3 normal, vec3 viewDir);
vec3 calculateDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir);

float calculateDirectionShadows(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
float calculateOmniShadows(vec3 lightPos);

void main()
{
    vec3 viewDir = normalize(fs_in.TangentCamPos - fs_in.TangentFragPos);
	vec3 norm = texture(material.texture_normal1, fs_in.TexCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0);

    vec3 color = vec3(0.0f);
    for(int i = 0; i < NR_BASIC_LIGHTS; i++) {
		if (length(blight[i].ambient + blight[i].diffuse + blight[i].specular) > 0){
			color += calculateBasicLight(blight[i], norm, viewDir);
		}
	}
    for(int i = 0; i < NR_DIRECTION_LIGHTS; i++) {
		if (length(dlight[i].ambient + dlight[i].diffuse + dlight[i].specular) > 0){
			color += calculateDirectionLight(dlight[i], norm, viewDir);
		}
	}
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
		if (length(plight[i].ambient + plight[i].diffuse + plight[i].specular) > 0){
			color += calculatePointLight(plight[i], norm, viewDir);
		}
	}
    for(int i = 0; i < NR_SPOT_LIGHTS; i++) {
		if (length(slight[i].ambient + slight[i].diffuse + slight[i].specular) > 0){
			color += calculateSpotLight(slight[i], norm, viewDir);
		}
	}
    FragColor = vec4(color, 1.0f);

	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else 
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

vec3 calculateAmbient(vec3 lightAmbient) {
    vec3 ambient = lightAmbient * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
	return ambient;
}

vec3 calculateDiffuse(vec3 lightDirection, vec3 normal, vec3 lightDiffuse){
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse =  lightDiffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
	return diffuse;
}

vec3 calculateSpecular(vec3 lightDirection, vec3 viewDirection, vec3 normal, vec3 lightSpecular) {
    vec3 halfwayDir = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
    vec3 specular = lightSpecular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoords));
	return specular;
}

vec3 calculateBasicLight(BasicLight light, vec3 normal, vec3 viewDir) {
	vec3 tangentLightPos = fs_in.TBN * light.position;

    vec3 lightDir = normalize(tangentLightPos - fs_in.TangentFragPos);

	float shadow = calculateOmniShadows(light.position);

    vec3 ambient = calculateAmbient(light.ambient);
	vec3 diffuse = calculateDiffuse(lightDir, normal, light.diffuse) * (1 - shadow);
	vec3 specular = calculateSpecular(lightDir, viewDir, normal, light.specular) * (1 - shadow);

    vec3 finalColor = ambient + diffuse + specular;
    return finalColor;
}

vec3 calculateDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

	vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fs_in.FragPos, 1.0); 
	float shadow = calculateDirectionShadows(fragPosLightSpace, normal, lightDir);

    vec3 ambient = calculateAmbient(light.ambient);
	vec3 diffuse = calculateDiffuse(lightDir, normal, light.diffuse) * (1 - shadow);
	vec3 specular = calculateSpecular(lightDir, viewDir, normal, light.specular) * (1 - shadow);

    vec3 finalColor = ambient + diffuse + specular;
    return finalColor;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir) {
	vec3 tangentLightPos = fs_in.TBN * light.position;

    vec3 lightDir = normalize(tangentLightPos - fs_in.TangentFragPos);
    float dist = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (gamma ? dist * dist : dist));

	float shadow = calculateOmniShadows(light.position);

    vec3 ambient = calculateAmbient(light.ambient);
	vec3 diffuse = calculateDiffuse(lightDir, normal, light.diffuse) * (1 - shadow);
	vec3 specular = calculateSpecular(lightDir, viewDir, normal, light.specular) * (1 - shadow);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

    return (diffuse + specular);
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
	vec3 tangentLightPos = fs_in.TBN * light.position;

    vec3 lightDir = normalize(tangentLightPos - fs_in.TangentFragPos);
    float dist = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (gamma ? dist * dist : dist));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = calculateAmbient(light.ambient);
	vec3 diffuse = calculateDiffuse(lightDir, normal, light.diffuse);
	vec3 specular = calculateSpecular(lightDir, viewDir, normal, light.specular);

    vec3 finalColor = ambient*attenuation + diffuse*intensity*attenuation + specular*intensity*attenuation;

    return finalColor;
}

float calculateDirectionShadows(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
	//perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transfor to [0, 1] range
	projCoords = projCoords * 0.5 + 0.5;

	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	// bias to prevent acne
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	// texel size to average shadow color;
	vec2 texelSize = 1.0 / textureSize(material.texture_shadow_direction1, 0);
	float pcfShadow = 0.0;
	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			//closest depth of 9 texels
			float pcfDepth = texture(material.texture_shadow_direction1, projCoords.xy + vec2(x, y) * texelSize).r;
			pcfShadow += (currentDepth - bias > pcfDepth) && (projCoords.z < 1.0) ? 1.0 : 0.0;
		}
	}
	pcfShadow /= 9.0;

	return pcfShadow;
}

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float calculateOmniShadows(vec3 lightPos){
	vec3 fragToLight = fs_in.FragPos - lightPos;
	//get linear depth between fragment and light position
	float currentDepth = length(fragToLight);

	float shadow = 0.0;
	float bias = 0.05;
	float viewDistance = length(camPos - fs_in.FragPos);
	float diskRadius = (1.0 + (viewDistance / 25.0)) / 25.0;
	float offset = 0.1;
	int samples = 20;
	for (int i = 0; i < samples; ++i){
				float closestDepth = texture(material.texture_shadow_cube1, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
				//transform depth to [0, far]
				closestDepth *= 25.0;
				// bias to prevent acne
				if (currentDepth - bias > closestDepth)
					shadow +=  1.0;
	}

	return shadow /= float(samples);
}
