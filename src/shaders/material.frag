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
	vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
    float opacity;
	float reflectivity;
	float refractionIndex;
};

uniform Material material;

in VS_OUT {
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
} fs_in;

out vec4 FragColor;

void main()
{
	// obtain normal from normal map in range [0,1]
	// only normalized this for nanosuit. shouldnt have to but for some reason did.
    // transform normal vector to range [-1,1]
    vec3 normal = normalize(fs_in.Normal);  // this normal is in tangent space
   
    // get diffuse color for ambient and diffuse
    // final ambient
    vec3 ambient = dlight[0].color * dlight[0].ambient * material.ambient.rgb;

    // light diffuse
    vec3 lightDir = normalize(-dlight[0].direction);
    float diff = max(dot(lightDir, normal), 0.0);
	// final diffuse
    vec3 diffuse = dlight[0].color * dlight[0].diffuse * diff * material.diffuse.rgb;

    // light specular 
    vec3 viewDir = normalize(camPos - fs_in.FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

	// object specular
	// final specular
    vec3 specular = dlight[0].color * dlight[0].specular * spec * material.specular.rgb;
    //vec3 specular = vec3(0.2) * spec;

	vec3 lighting = ambient + diffuse + specular;

    FragColor = vec4(lighting, 1.0);
}

