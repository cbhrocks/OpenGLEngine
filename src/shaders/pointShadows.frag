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

uniform samplerCube shadowCubeMap;
uniform float shadowFar;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

vec3 sampleOffsetDirections[20] = vec3[] (
	vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float ShadowCalculation(vec3 fragPos, vec3 lightPos)
{
	// distance and direction from fragment position to light source
	vec3 fragToLight = fragPos - lightPos;
	// closest depth of any object retrieved from the shadowmap
	float closestDepth = texture(shadowCubeMap, fragToLight).r;

	// transform from texture range [0, 1] to [0, shadowFar].
	closestDepth *= shadowFar;

	float currentDepth = length(fragToLight);

	//basic shadow calculation below.
	//float bias = 0.05;
	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	// PCF (percentage-closer filtering) below
	float shadow = 0.0;
	float bias   = 0.15;
	int samples  = 20;
	float viewDistance = length(camPos - fragPos);
	float diskRadius = (1.0 + (viewDistance / shadowFar)) / 25.0; // scale radius based on view distance to object
	for(int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(shadowCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= shadowFar;   // Undo mapping [0;1]
		if(currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples);  
	
	// PCF: average shadow texels for smooth shadows.
	return shadow;
}

// uses basic blinn phong lighting algorithm
void main()
{
	// obtain normal from normal map in range [0,1]
	// only normalized this for nanosuit. shouldnt have to but for some reason did.
    // transform normal vector to range [-1,1]
    vec3 normal = normalize(fs_in.Normal);  // this normal is in tangent space
	vec3 color = texture(material.texture_diffuse, fs_in.TexCoords).rgb;
   
    // get diffuse color for ambient and diffuse
    // final ambient
    vec3 ambient = plight[0].ambient * color;

    // light diffuse
    vec3 lightDir = normalize(plight[0].position - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
	// final diffuse
    vec3 diffuse = plight[0].color * plight[0].diffuse * diff;

    // light specular 
    vec3 viewDir = normalize(camPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);

	// object specular
	// final specular
    vec3 specular = plight[0].color * plight[0].specular * spec;
    //vec3 specular = vec3(0.2) * spec;

	float shadow = ShadowCalculation(fs_in.FragPos, plight[0].position);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}
