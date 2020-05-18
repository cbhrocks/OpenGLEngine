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

uniform sampler2D shadowMap;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// coords not passed through gl_position, so need to do perspective divide manually
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to range [0, 1]
	projCoords = projCoords * 0.5 + 0.5;

	// closest depth of any object retrieved from the shadowmap
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// depth of the current fragment
	float currentDepth = projCoords.z;

	// prevent shadow acne by applying a bias in the direction of the surface normal.
	// the bias will change based on the angle between the light and the surface. max of 0.05, min of 0.005
	float bias = max(0.05 * (1.0 - dot(fs_in.Normal, dlight[0].direction)), 0.005);
	
	//basic shadow calculation below.
	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	// PCF: average shadow texels for smooth shadows.
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; ++x){
		for (int y = -1; y <=1; ++y){
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 9.0;

	if (projCoords.z > 1.0) {
		shadow = 0.0;
	}

	return shadow;
}

// uses basic phong lighting algorithm
void main()
{
	// obtain normal from normal map in range [0,1]
	// only normalized this for nanosuit. shouldnt have to but for some reason did.
    // transform normal vector to range [-1,1]
    vec3 normal = normalize(fs_in.Normal);  // this normal is in tangent space
	vec3 color = texture(material.texture_diffuse, fs_in.TexCoords).rgb;
   
    // get diffuse color for ambient and diffuse
    // final ambient
    vec3 ambient = dlight[0].color * dlight[0].ambient * color;

    // light diffuse
    vec3 lightDir = normalize(-dlight[0].direction);
    float diff = max(dot(lightDir, normal), 0.0);
	// final diffuse
    vec3 diffuse = dlight[0].color * dlight[0].diffuse * diff;

    // light specular 
    vec3 viewDir = normalize(camPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);

	// object specular
	// final specular
    vec3 specular = dlight[0].color * dlight[0].specular * spec;
    //vec3 specular = vec3(0.2) * spec;

	float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	//vec3 lighting = (diffuse) * color;

    FragColor = vec4(lighting, 1.0);
}
