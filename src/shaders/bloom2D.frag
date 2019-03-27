#version 330 core
  
in vec2 TexCoords;

uniform sampler2D sceneTex;
uniform sampler2D bloomTex;
//uniform float exposure;

layout (std140) uniform Scene
{
	float time;
	bool gamma;
	float exposure;
};

out vec4 FragColor;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(sceneTex, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomTex, TexCoords).rgb;
    hdrColor += bloomColor; // additive blending

    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}