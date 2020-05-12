#version 330 core
out vec4 FragColor;
  
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

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

uniform PointLight plight;

layout (std140) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 camPos;
};

vec2 CalcTexCoord()
{
	return (gl_FragCoord / 1024.0f).xy;
}

void main()
{             
	vec2 tex_coords = CalcTexCoord();
    // retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, tex_coords).rgb;
    vec3 Normal = texture(gNormal, tex_coords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, tex_coords).rgb;
    float Specular = texture(gAlbedoSpec, tex_coords).a;
    
    // then calculate lighting as usual
    vec3 ambient = Albedo * plight.ambient * plight.color; // hard-coded ambient component
    vec3 viewDir = normalize(camPos - FragPos);

	// diffuse
	vec3 lightDir = normalize(plight.position - FragPos);
	vec3 diffuse = plight.color * plight.diffuse * max(dot(Normal, lightDir), 0.0) * Albedo;
	// specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
	vec3 specular = plight.color * plight.specular * spec * Specular;
	// attenuation
	float distance = length(plight.position - FragPos);
	float attenuation = 1.0 / (1.0 + plight.linear * distance + plight.quadratic * distance * distance);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	vec3 lighting = ambient + diffuse + specular;

    FragColor = vec4(lighting, 1.0f);
}
