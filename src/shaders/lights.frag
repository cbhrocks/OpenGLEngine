#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//light data
uniform Light light;

//camera data
uniform vec3 camPos;

//object data
uniform Material material;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;


void main()
{
    vec3 ambient = light.diffuse * vec3(texture(material.diffuse, TexCoords));

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

    vec3 viewDir = normalize(camPos - norm);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(finalColor, 1.0);
}
