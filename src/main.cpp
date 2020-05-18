#define _CRT_SECURE_NO_WARNINGS
#define RESOLUTION 512
#define TARGET_FPS 30                // controls spin update rate
#define TIME_BETWEEN_UPDATES 0.015   // seconds between motion updates
#define PRINT_FPS_INTERVAL 10.0f
#define PI 3.14159f

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "renderer.h"
#include "Scene.h"
#include "LightManager.h"
#include "counter.h"
#include "DrawObj.h"
#include "Sphere.h"
#include "Icosphere.h"
#include "scenes.h"
#include "debug_control.h"


// opengl function for handling debug output
void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

typedef struct
{
	GLFWwindow* window;
	Renderer render;
	Scene* scene;
    DebugControl* debugControl;
    int id;
} Slot;

int keys[1024], mouse[24], mouseX, mouseY;
bool modKeys[4];

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static const char* get_action_name(int action)
{
    switch (action)
    {
        case GLFW_PRESS:
            return "pressed";
        case GLFW_RELEASE:
            return "released";
        case GLFW_REPEAT:
            return "repeated";
    }

    return "caused unknown action";
}

static const char* get_mods_name(int mods)
{
    static char name[512];

    if (mods == 0)
        return " no mods";

    name[0] = '\0';

    if (mods & GLFW_MOD_SHIFT)
        strcat(name, " shift");
    if (mods & GLFW_MOD_CONTROL)
        strcat(name, " control");
    if (mods & GLFW_MOD_ALT)
        strcat(name, " alt");
    if (mods & GLFW_MOD_SUPER)
        strcat(name, " super");
    //if (mods & GLFW_MOD_CAPS_LOCK)
    //    strcat(name, " capslock-on");
    //if (mods & GLFW_MOD_NUM_LOCK)
    //    strcat(name, " numlock-on");

    return name;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }

    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);

    const char* name = glfwGetKeyName(key, scancode);
    const int lastAction = keys[key];

    if (name)
    {
        printf("%i at %0.3f: Key 0x%04x Scancode 0x%04x (%s) (with%s) was %s\n",
               slot->id, glfwGetTime(), key, scancode,
               //get_key_name(key),
               name,
               get_mods_name(mods),
               get_action_name(action));
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = GLFW_PRESS;
        else if (action == GLFW_REPEAT)
            keys[key] = GLFW_REPEAT;
        else if (action == GLFW_RELEASE)
            keys[key] = GLFW_RELEASE;
    }
    if (mods >= 0 && mods < 1024)
    {
        if (action == GLFW_PRESS)
            modKeys[mods] = true;
        else if (action == GLFW_RELEASE)
            modKeys[mods] = false;
    }

    //handle key callback
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (keys[GLFW_KEY_P]){
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
        slot->render.toggleWireframeMode();
	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		if (slot->render.getGammaCorrection() == 1.0f)
			slot->render.setGammaCorrection(2.2f);
		else
			slot->render.setGammaCorrection(1.0f);
	}
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);
    printf("%i at %0.3f: Mouse button %i (with%s) was %s\n",
           slot->id, glfwGetTime(), button,
           //get_button_name(button),
           get_mods_name(mods),
           get_action_name(action));

    if (button >= 0 && button < 1024)
    {
        if (action == GLFW_PRESS)
            mouse[button] = GLFW_PRESS;
        else if (action == GLFW_REPEAT)
            mouse[button] = GLFW_REPEAT;
        else if (action == GLFW_RELEASE)
            mouse[button] = GLFW_RELEASE;
    }

    switch (button) {
    case GLFW_MOUSE_BUTTON_MIDDLE:
        if (action == GLFW_PRESS)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (action == GLFW_RELEASE)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    }
}

static void scroll_callback(GLFWwindow* window, double x, double y)
{
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);
    printf("%i at %0.3f: Scroll: %0.3f %0.3f\n",
           slot->id, glfwGetTime(), x, y);
}

static void doMovement(Scene* scene) {
    GLfloat cameraSpeed = 0.1f;
	Camera* camera = scene->getActiveCamera();
    if(keys[GLFW_KEY_W]){
		camera->setPosition(camera->getPosition() + cameraSpeed * camera->getFront());
        //printf("forward\n");
    }
    if(keys[GLFW_KEY_S]){
		camera->setPosition(camera->getPosition() - cameraSpeed * camera->getFront());
        //printf("backward\n");
    }
    if(keys[GLFW_KEY_A]){
		camera->setPosition(camera->getPosition() - glm::normalize(glm::cross(camera->getFront(), camera->getUp())) * cameraSpeed);
        //printf("left\n");
    }
    if(keys[GLFW_KEY_D]){
		camera->setPosition(camera->getPosition() + glm::normalize(glm::cross(camera->getFront(), camera->getUp())) * cameraSpeed);
        //printf("right\n");
    }
    //if (keys[GLFW_KEY_X] && modKeys[GLFW_MOD_ALT]){
    //    scene->cameraLook = glm::vec3(-1,0,0);
    //    scene->cameraUp = glm::vec3(0,1,0);
    //    printf("-X\n");
    //}
    //else if (keys[GLFW_KEY_X]){
    //    scene->cameraLook = glm::vec3(1,0,0);
    //    scene->cameraUp = glm::vec3(0,1,0);
    //    printf("X\n");
    //}
    //if (keys[GLFW_KEY_Y] && modKeys[GLFW_MOD_ALT]){
    //    scene->cameraLook = glm::vec3(0,-1,0);
    //    scene->cameraUp = glm::vec3(0,0,-1);
    //    printf("-Y\n");
    //}
    //else if (keys[GLFW_KEY_Y]){
    //    scene->cameraLook = glm::vec3(0,1,0);
    //    scene->cameraUp = glm::vec3(0,0,1);
    //    printf("Y\n");
    //}
    //if (keys[GLFW_KEY_Z] && modKeys[GLFW_MOD_ALT]){
    //    scene->cameraLook = glm::vec3(0,0,1);
    //    scene->cameraUp = glm::vec3(0,1,0);
    //    printf("-Z\n");
    //}
    //else if (keys[GLFW_KEY_Z]){
    //    scene->cameraLook = glm::vec3(0,0,-1);
    //    scene->cameraUp = glm::vec3(0,1,0);
    //    printf("Z\n");
    //}
}

// this function is called when a monitor is connected or disconnected.
static void monitor_callback(GLFWmonitor* monitor, int event)
{
    if (event == GLFW_CONNECTED)
    {
        int x, y, widthMM, heightMM;
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwGetMonitorPos(monitor, &x, &y);
        glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);

        printf("%08x at %0.3f: Monitor (%ix%i at %ix%i, %ix%i mm) was connected\n",
               glfwGetTime(),
               glfwGetMonitorName(monitor),
               mode->width, mode->height,
               x, y,
               widthMM, heightMM);
    }
    else if (event == GLFW_DISCONNECTED)
    {
        printf("%08x at %0.3f: Monitor was disconnected\n",
               glfwGetTime(),
               glfwGetMonitorName(monitor));
    }
}

static void window_refresh_callback(GLFWwindow* window)//, int focused)
{
    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);
    glfwSwapBuffers(slot->window);
}

static void window_focus_callback(GLFWwindow* window, int focused)
{
    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);
    printf("%i at %0.3f: Window %s\n",
           slot->id, glfwGetTime(),
           focused ? "focused" : "defocused");
}

bool firstMouse = true;
bool firstMouseMiddlePos = true;
static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

	if (firstMouse) {
		mouseX = x;
		mouseY = y;
		firstMouse = false;
	}

	if (mouse[GLFW_MOUSE_BUTTON_MIDDLE] == GLFW_PRESS) {
		if (firstMouseMiddlePos) {
			mouseX = x;
			mouseY = y;
			firstMouseMiddlePos = false;
		}

		Slot* slot = (Slot*)glfwGetWindowUserPointer(window);
		Camera* camera = slot->scene->getActiveCamera();


		double xOff = x - mouseX;
		double yOff = mouseY - y;

		xOff *= 0.4f; //this->MouseSensitivity;
		yOff *= 0.4f; //this->MouseSensitivity;

		float new_yaw = camera->getYaw() + xOff;
		float new_pitch = camera->getPitch() + yOff;

		printf("%i at %0.3f: Cursor position: %f %f Position offset: %f %f Pitch and Yaw: %f %f\n",
			slot->id, glfwGetTime(), x, y, xOff, yOff, new_pitch, new_yaw);

		mouseX = x;
		mouseY = y;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (new_pitch > 89.0f)
			new_pitch = 89.0f;
		if (new_pitch < -89.0f)
			new_pitch = -89.0f;

		camera->setPitch(new_pitch);
		camera->setYaw(new_yaw);

		slot->scene->getActiveCamera()->updateVectors(slot->scene->getUp());
	}
    if (mouse[GLFW_MOUSE_BUTTON_MIDDLE] == GLFW_RELEASE) {
        firstMouseMiddlePos = true;
    }
}

static void cursor_enter_callback(GLFWwindow* window, int entered)
{
    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);
    printf("%i at %0.3f: Cursor %s window\n",
           slot->id, glfwGetTime(),
           entered ? "entered" : "left");
}

static void window_close_callback(GLFWwindow* window)
{
    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);
    printf("%i at %0.3f: Window close\n",
           slot->id, glfwGetTime());

    //glfwSetWindowShouldClose(window, slot->closeable);
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);

    printf("%i at %0.3f: Window size: %i %i\n",
           slot->id, glfwGetTime(), width, height);
}

static void window_position_callback(GLFWwindow* window, int x, int y)
{
    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);
    printf("%i at %0.3f: Window position: %i %i\n",
           slot->id, glfwGetTime(), x, y);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);
    slot->render.setDimensions(width, height);
}

int main(int argc, char** argv)
{
    // initialize glfw
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);


    //GLFWmonitor* monitor = NULL;

	// initialize opengl version
    const char* glsl_version = "#version 330 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    size_t width, height;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);

	width = 1024;
	height = 1024;
	//Create the window
    GLFWwindow* window = glfwCreateWindow(width, height, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        printf("window creation failed....\n");
        exit(EXIT_FAILURE);
    }
    printf("window created!\n");

    //load glad
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    std::cout << "Loaded OpenGL " << GLVersion.major << "." << GLVersion.minor << std::endl;
    std::cout << "opengl version: " << glGetString(GL_VERSION) << std::endl;

	// initialize debug output must be after glad has been loaded
	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

    glfwSwapInterval(1);


    // initialize imgui

    //initalize scene


    Slot slot;// = new Slot;
    slot.scene = new Scene();
    slot.window = window;
    slot.render = Renderer(width, height);


	//slot.scene.setFBOManager(fbom)
	//slot.render.setTBM(tbm);
	GBuffer* gBuffer = new GBuffer(width, height);
	slot.render.setGBuffer(gBuffer);
    slot.id = 0;

    //create callbacks
    glfwSetWindowUserPointer(window, &slot);

    glfwSetMonitorCallback(monitor_callback);
    glfwSetKeyCallback(slot.window, key_callback);
    glfwSetWindowRefreshCallback(slot.window, window_refresh_callback);
    glfwSetWindowFocusCallback(slot.window, window_focus_callback);
    glfwSetCursorPosCallback(slot.window, cursor_position_callback);
    glfwSetCursorEnterCallback(slot.window, cursor_enter_callback);
    glfwSetWindowCloseCallback(slot.window, window_close_callback);
    glfwSetWindowSizeCallback(slot.window, window_size_callback);
    glfwSetWindowPosCallback(slot.window, window_position_callback);
    glfwSetMouseButtonCallback(slot.window, mouse_button_callback);
    glfwSetScrollCallback(slot.window, scroll_callback);
    glfwSetFramebufferSizeCallback(slot.window, framebuffer_size_callback);

    // this needs to be below other callbacks. it chains callbacks
    slot.debugControl = new DebugControl(glsl_version, slot.window, slot.scene, &slot.render);

    // load stuff into the scene
    setupBasic(slot.scene, &slot.render);

    float lastTime = glfwGetTime();
    float lastPrint = lastTime;
    Counter<float> counter;

    printf("entering loop!\n");

    while (!glfwWindowShouldClose(slot.window))
    {
        glfwPollEvents();

        slot.debugControl->onFrame();

        float currentTime = glfwGetTime();
        float elapsedTime = currentTime - lastTime;
        float sinceLastPrint = currentTime - lastPrint;

        if(sinceLastPrint > PRINT_FPS_INTERVAL)
        {
            lastPrint = currentTime;
            printf("time %.2fms\n", counter.getAverage()*1000);
        }
        counter.push(elapsedTime);

		slot.render.setTime(currentTime);
		slot.render.preRender(slot.scene);
		slot.render.render(slot.scene);
		//slot.render.renderToGBuffer(slot.scene);
        //slot.render.renderHighlight(slot.scene);
		//slot.render.renderLights(slot.scene);
		//slot.render.renderVertexNormalLines(slot.scene);
		//slot.render.renderFaceNormalLines(slot.scene);
		//slot.render.renderTBNLines(slot.scene);
		//slot.render.renderDepth(slot.scene);

        doMovement(slot.scene);

		for (auto& updateFunction_it : slot.scene->getUpdateFunctions()) {
			updateFunction_it.second(slot.scene);
		}
        
        lastTime = currentTime;
        //break;
        slot.debugControl->render();

        glfwSwapBuffers(slot.window);
    }

    delete slot.debugControl;

    glfwDestroyWindow(slot.window);

    glfwTerminate();

    //delete slot

	return EXIT_SUCCESS;
}