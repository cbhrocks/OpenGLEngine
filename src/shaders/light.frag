#version 330 core

struct Light {
    vec3 position;
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
};

uniform Light light;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main()
{
	vec3 ambient = light.color * light.ambient;
	vec3 diffuse = light.color * light.diffuse;
	vec3 specular = light.color * light.specular;
    FragColor = vec4(ambient + diffuse + specular, 1.0);

	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else 
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
