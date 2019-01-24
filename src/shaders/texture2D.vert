#version 330

uniform mat4 P;  //projection matrix
uniform mat4 C;  //camera matrix
uniform mat4 M;  //modelview matrix: M = mat4(1.0);// * mR * mT
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
uniform vec3 caeraUp;    //camera up vector
uniform vec4 lightPos;    //light position
uniform vec4 camPos;      //camera position

layout (location = 0) in vec3 pos;
in vec3 colorIn;

out vec3 TexCoords;

void main()
{
    vec3 tp = (pos + vec3(1, 1, 1))/2;
    gl_Position = vec4(pos, 1.0);

    TexCoords = tp;
}
