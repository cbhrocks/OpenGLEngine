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
#include "counter.h"

using namespace std;

typedef struct
{
	GLFWwindow* window;
	Renderer render;
	Scene* scene;
    int id;
} Slot;

int keys[1024], mouseX, mouseY;
GLfloat yaw, pitch;
bool modKeys[4];

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
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
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
        slot->scene->setGammaCorrection(!slot->scene->getGammaCorrection());
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);
    printf("%i at %0.3f: Mouse button %i (with%s) was %s\n",
           slot->id, glfwGetTime(), button,
           //get_button_name(button),
           get_mods_name(mods),
           get_action_name(action));
}

static void scroll_callback(GLFWwindow* window, double x, double y)
{
    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);
    printf("%i at %0.3f: Scroll: %0.3f %0.3f\n",
           slot->id, glfwGetTime(), x, y);
}

static void doMovement(Scene* scene) {
    GLfloat cameraSpeed = 0.1f;
	Camera* camera = scene->getActiveCamera();
    if(keys[GLFW_KEY_W]){
		camera->setPosition(camera->getPosition() + cameraSpeed * camera->getLook());
        //printf("forward\n");
    }
    if(keys[GLFW_KEY_S]){
		camera->setPosition(camera->getPosition() - cameraSpeed * camera->getLook());
        //printf("backward\n");
    }
    if(keys[GLFW_KEY_A]){
		camera->setPosition(camera->getPosition() - glm::normalize(glm::cross(camera->getLook(), camera->getUp())) * cameraSpeed);
        //printf("left\n");
    }
    if(keys[GLFW_KEY_D]){
		camera->setPosition(camera->getPosition() + glm::normalize(glm::cross(camera->getLook(), camera->getUp())) * cameraSpeed);
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
static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);

    if (firstMouse){
        mouseX = x;
        mouseY = y;
        firstMouse = false;
    }

    double xOff = x - mouseX;
    double yOff = mouseY - y;

    yaw += xOff;
    pitch += yOff;

    printf("%i at %0.3f: Cursor position: %f %f Position offset: %f %f Pitch and Yaw: %f %f\n",
           slot->id, glfwGetTime(), x, y, xOff, yOff, pitch, yaw);

    mouseX = x;
    mouseY = y;

    xOff *= 0.1f; //this->MouseSensitivity;
    yOff *= 0.1f; //this->MouseSensitivity;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    //glm::vec3 camLook = slot->scene.getCameraLook();
    glm::vec3 newLook;

    newLook.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newLook.y = sin(glm::radians(pitch));
    newLook.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    // Also re-calculate the Right and Up vector
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    glm::vec3 right = glm::normalize(glm::cross(newLook, slot->scene->getUp()));  
    glm::vec3 camUp = glm::normalize(glm::cross(right, newLook));

	slot->scene->getActiveCamera()->setLook(glm::normalize(newLook));
    slot->scene->getActiveCamera()->setUp(camUp);

    printf("new look: %f, %f, %f\n", newLook.x, newLook.y, newLook.z);
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

    slot->render.setRes(width, height);
}

static void window_position_callback(GLFWwindow* window, int x, int y)
{
    Slot* slot = (Slot*) glfwGetWindowUserPointer(window);
    printf("%i at %0.3f: Window position: %i %i\n",
           slot->id, glfwGetTime(), x, y);
}

int main(int argc, char** argv)
{
    GLFWmonitor* monitor = NULL;
    int width, height;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //scene.currentRes[0] = RESOLUTION;
    //scene.currentRes[1] = RESOLUTION;

    //printf("res: %d x %d\n", scene.currentRes[0], scene.currentRes[1]);

    glfwSetMonitorCallback(monitor_callback);

    monitor = glfwGetPrimaryMonitor();

    if (false)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    }
    else
    {
        width  = 640;
        height = 480;
    }

    if (monitor)
    {
        printf("Creating full screen window (%ix%i on %s)\n",
                width, height,
                glfwGetMonitorName(monitor));
    }
    else
    {
        printf("Creating windowed mode window (%ix%i)\n",
                width, height);
    }

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
        cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    cout << "Loaded OpenGL " << GLVersion.major << "." << GLVersion.minor << endl;
    cout << "opengl version: " << glGetString(GL_VERSION) << endl;


    glfwSwapInterval(1);

    //initalize scene

    Slot slot;// = new Slot;
    slot.scene = new Scene();
    slot.window = window;
    slot.render = Renderer(width, height);
    slot.id = 0;

    pitch = 0.0f;
    yaw = 0.0f;

    //create callbacks
    glfwSetWindowUserPointer(window, &slot);

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

    float lastTime = glfwGetTime();
    float lastPrint = lastTime;
    Counter<float> counter;

    printf("entering loop!\n");

    while (!glfwWindowShouldClose(slot.window))
    {
        float currentTime = glfwGetTime();
        float elapsedTime = currentTime - lastTime;
        float sinceLastPrint = currentTime - lastPrint;

        if(sinceLastPrint > PRINT_FPS_INTERVAL)
        {
            lastPrint = currentTime;
            printf("time %.2fms\n", counter.getAverage()*1000);
        }
        counter.push(elapsedTime);

		slot.scene->onFrame();
		slot.render.preRender(slot.scene);
		slot.render.renderTexture(slot.scene);
        //slot.render.renderHighlight(slot.scene);
		slot.render.postRender(slot.scene);

        glfwSwapBuffers(slot.window);

        glfwPollEvents();
        doMovement(slot.scene);
        //glfwWaitEvents();
        //render.render(scene);
        
        slot.scene->timeStep(currentTime);

        lastTime = currentTime;
        //break;
    }

    glfwDestroyWindow(slot.window);

    glfwTerminate();

    //delete slot

	return EXIT_SUCCESS;
}