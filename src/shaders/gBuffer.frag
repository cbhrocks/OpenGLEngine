#version 330 core

// could not get the gPosition or gNormal to work as vec3's. only solution was to change to vec4
// the global position of each frag
layout (location = 0) out vec3 gPosition;
// the normal vector of each frag in world space
layout (location = 1) out vec3 gNormal;

// the combined albedo color and specular intensity
layout (location = 2) out vec4 gAlbedoSpec;


in VS_OUT {
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
} fs_in;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
};

uniform Material material;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = fs_in.FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(fs_in.Normal);
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(material.diffuse, fs_in.TexCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(material.specular, fs_in.TexCoords).r;
}  