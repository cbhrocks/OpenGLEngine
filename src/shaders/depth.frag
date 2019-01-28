#version 330

uniform float near;               //near rendering plane
uniform float far;                //far rendering plane

uniform int toIntRange; //divide by 30 to get depth in 0-255 range

smooth in float distance;

vec4 toon_edges = vec4(0.1, 0.5, 0.8, 0.8);
float outline_edge = 0.3;

float LinearizeDepth(float depth){
    //makes coordinates NDC
    float z = depth * 2.0 - 1.0;

    // makes linear, is inverse of (1/z-1/near)/(1/far-1/near) non linear depth function
    return (2.0 * near * far) / (far + near - z * (far - near)); 
}

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z);///far;

	fragColor = vec4(vec3(distance), 1.0);
	if(toIntRange != 0)
		fragColor = vec4(vec3(distance / 50), 1.0);
}
