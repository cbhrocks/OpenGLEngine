#version 330

uniform sampler2D texture1;
uniform samplerCube skybox;
uniform vec3 camPos;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
	vec3 I = normalize(FragPos - camPos);
	vec3 R = reflect(I, normalize(Normal));
	//FragColor = texture(texture1, TexCoords);
	FragColor = mix(vec4(texture(skybox, R).rgb, 1.0), texture(texture1, TexCoords), 0.0);
}
