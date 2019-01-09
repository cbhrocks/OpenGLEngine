#version 330

uniform mat4 P;  //projection matrix
uniform mat4 C;  //camera matrix
uniform mat4 M;  //modelview matrix: M = C * mR * mT
uniform mat3 N;  //inverse transpose of upperleft 3x3 of M
uniform mat4 L;  //light rotation matrix

uniform float elapsedTime;        //time in seconds since simulation began
uniform float near;               //near rendering plane
uniform float far;                //far rendering plane
uniform float fov;                //field of view
uniform float cursorScrollAmount; //delta scroll amount
uniform vec2 resolution;          //view resolution in pixels
uniform vec2 cursorAbsolutePos;   //absolute cursor position in pixels
uniform vec2 cursorDragAmount;    //tracks amount of x,y change while the mouse button is down (i.e. amount of drag)
uniform vec2 lastClickPos;        //where the last mouse click happened
uniform vec2 lastFrameDragPos;    //where the cursor was on the previous frame while the mouse button is down
uniform int  mouseButtonDown;     //tracks if the mouse button is down

uniform vec3 modelCenter; //center of the model (might be useful for rotations)
uniform vec3 lookAtPos;   //point in 3D space the camera is point towards
uniform vec3 cameraUp;    //camera up vector
uniform vec4 lightPos;    //light position
uniform vec4 camPos;      //camera position

uniform int toIntRange; //divide by 30 to get depth in 0-255 range

smooth in vec4 smoothColor;
smooth in vec3 smoothPos;
smooth in vec3 smoothNorm;
smooth in float distance;

layout(location = 0) out vec4 fragColor;

vec4 toon_edges = vec4(0.1, 0.5, 0.8, 0.8);
float outline_edge = 0.3;

float LinearizeDepth(float depth){
    //makes coordinates NDC
    float z = depth * 2.0 - 1.0;

    // makes linear, is inverse of (1/z-1/near)/(1/far-1/near) non linear depth function
    return (2.0 * near * far) / (far + near - z * (far - near)); 
}

//float deLinearizeDepth(float depth){
    //unNDC coordinates
    //float z = (depth+1)/2; 

    //makes depth a non linear function
    //return (1/z - 1/near)/(1/far - 1/near);
//}

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z);///far;
//    //float depth = gl_FragCoord.z;
//    fragColor = vec4(vec3(depth), 1.0f);
//
	fragColor = vec4(vec3(distance), 1.0);
	if(toIntRange != 0)
		fragColor = vec4(vec3(distance / 50), 1.0);
}
