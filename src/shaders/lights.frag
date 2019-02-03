#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct BasicLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirectionLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

//light data
#define NR_BASIC_LIGHTS 1
#define NR_DIRECTION_LIGHTS 1
#define NR_POINT_LIGHTS 1
#define NR_SPOT_LIGHTS 1
uniform BasicLight blight[NR_BASIC_LIGHTS]; // basic light
uniform DirectionLight dlight[NR_DIRECTION_LIGHTS]; // directional light
uniform PointLight plight[NR_POINT_LIGHTS]; // point light
uniform SpotLight slight[NR_SPOT_LIGHTS]; // spot light

//camera data
uniform vec3 camPos;

//object data
uniform Material material;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

vec3 calculateBasicLight(BasicLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculateDirectionLight(DirectionLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 viewDir = normalize(camPos - FragPos);
    vec3 norm = normalize(Normal);

    vec3 color = vec3(0.0f);
    //for(int i = 0; i < NR_BASIC_LIGHTS; i++)
    //    color += calculateBasicLight(blight[i], norm, FragPos, viewDir);
    //for(int i = 0; i < NR_DIRECTION_LIGHTS; i++)
    //    color += calculateDirectionLight(dlight[i], norm, FragPos, viewDir);
    //for(int i = 0; i < NR_POINT_LIGHTS; i++)
    //    color += calculatePointLight(plight[i], norm, FragPos, viewDir);
    for(int i = 0; i < NR_SPOT_LIGHTS; i++)
        color += calculateSpotLight(slight[i], norm, FragPos, viewDir);
    FragColor = vec4(color, 1.0f);
}

vec3 calculateBasicLight(BasicLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 ambient = light.diffuse * vec3(texture(material.diffuse, TexCoords));

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse =  light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

    vec3 finalColor = ambient;// + diffuse + specular;
    return finalColor;
}

vec3 calculateDirectionLight(DirectionLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 ambient = light.diffuse * vec3(texture(material.diffuse, TexCoords));

    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse =  light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

    vec3 finalColor = ambient + diffuse + specular;
    return finalColor;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    vec3 ambient = light.diffuse * vec3(texture(material.diffuse, TexCoords));

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse =  light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    vec3 finalColor = ambient*attenuation + diffuse*attenuation + specular*attenuation;
    //vec3 finalColor = ambient + diffuse + specular;
    return finalColor;
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    vec3 ambient = light.diffuse * vec3(texture(material.diffuse, TexCoords));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse =  light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    vec3 finalColor = ambient*attenuation + diffuse*intensity*attenuation + specular*intensity*attenuation;

    return finalColor;
}
