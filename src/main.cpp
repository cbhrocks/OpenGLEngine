#define _CRT_SECURE_NO_WARNINGS
#define RESOLUTION 512
#define TARGET_FPS 30                // controls spin update rate
#define TIME_BETWEEN_UPDATES 0.015   // seconds between motion updates
#define PRINT_FPS_INTERVAL 10.0f
#define PI 3.14159f

#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderManager.h"
#include "renderer.h"
#include "scene.h"
#include "counter.h"
//#include "textBufferManager2D.h"

bool keys[1024];
bool modKeys[4];

class Program
{
public:
    static void error_callback(int error, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }
	
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Scene * s = reinterpret_cast<Scene *>(glfwGetWindowUserPointer(window));
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		if (key >= 0 && key < 1024)
		{
			if (action == GLFW_PRESS)
				keys[key] = true;
			else if (action == GLFW_RELEASE)
				keys[key] = false;
		}
		if (mods >= 0 && mods < 1024)
		{
			if (action == GLFW_PRESS)
				modKeys[mods] = true;
			else if (action == GLFW_RELEASE)
				modKeys[mods] = false;
		}
	}
	
	void doMovement(Scene* s)
	{
		GLfloat cameraSpeed = 0.01f;
		if(keys[GLFW_KEY_W]){
			s->cameraPos += cameraSpeed * s->cameraLook;
			printf("forward\n");
		}
		if(keys[GLFW_KEY_S]){
			s->cameraPos -= cameraSpeed * s->cameraLook;
			printf("backward\n");
		}
		if(keys[GLFW_KEY_A]){
			s->cameraPos -= glm::normalize(glm::cross(s->cameraLook, s->cameraUp)) * cameraSpeed;
			printf("left\n");
		}
		if(keys[GLFW_KEY_D]){
			s->cameraPos += glm::normalize(glm::cross(s->cameraLook, s->cameraUp)) * cameraSpeed;
			printf("right\n");
		}
		if (keys[GLFW_KEY_X] && modKeys[GLFW_MOD_ALT]){
			s->cameraLook = glm::vec3(-1,0,0);
			s->cameraUp = glm::vec3(0,1,0);
			printf("-X\n");
		}
		else if (keys[GLFW_KEY_X]){
			s->cameraLook = glm::vec3(1,0,0);
			s->cameraUp = glm::vec3(0,1,0);
			printf("X\n");
		}
		if (keys[GLFW_KEY_Y] && modKeys[GLFW_MOD_ALT]){
			s->cameraLook = glm::vec3(0,-1,0);
			s->cameraUp = glm::vec3(0,0,-1);
			printf("-Y\n");
		}
		else if (keys[GLFW_KEY_Y]){
			s->cameraLook = glm::vec3(0,1,0);
			s->cameraUp = glm::vec3(0,0,1);
			printf("Y\n");
		}
		if (keys[GLFW_KEY_Z] && modKeys[GLFW_MOD_ALT]){
			s->cameraLook = glm::vec3(0,0,1);
			s->cameraUp = glm::vec3(0,1,0);
			printf("-Z\n");
		}
		else if (keys[GLFW_KEY_Z]){
			s->cameraLook = glm::vec3(0,0,-1);
			s->cameraUp = glm::vec3(0,1,0);
			printf("Z\n");
		}
	}
	
	Program()
	{
		glfwSetErrorCallback(error_callback);
		
		if (!glfwInit())
			exit(EXIT_FAILURE);
		
		//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
		scene.currentRes[0] = RESOLUTION;
		scene.currentRes[1] = RESOLUTION;

        printf("res: %d x %d\n", scene.currentRes[0], scene.currentRes[1]);
		
		//Create the window
		window = glfwCreateWindow(scene.currentRes[0], scene.currentRes[1], "Simple example", NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			printf("window creation failed....\n");
			exit(EXIT_FAILURE);
		}
		printf("window created!\n");
		
		//load glad
		glfwMakeContextCurrent(window);
		gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
		printf("opengl version: %s\n", glGetString(GL_VERSION));
		
		glfwSwapInterval(1);
		
		//initalize scene
		render = Renderer(scene);
		
		glfwSetKeyCallback(window, key_callback);
		
		previousPos = glm::vec2(0);
		
		float lastTime = glfwGetTime();
		float lastPrint = lastTime;
		Counter<float> counter;
		
		printf("entering loop!\n");
		
        //render.render(scene);
        render.render2D(scene);
		
		glfwSetWindowUserPointer(window, &scene);
		
		while (!glfwWindowShouldClose(window))
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
			
			glfwPollEvents();
			doMovement(&scene);
			
			scene.timeStep(currentTime);
            render.render2D(scene);
            render.display2D(scene);
            //render.render(scene);

			glfwSwapBuffers(window);
			lastTime = currentTime;
            //break;
		}
		
		glfwDestroyWindow(window);
		
		glfwTerminate();
	}
	
private:
	GLFWwindow* window;
	Renderer render;
	Scene scene;
	
	//what does this do?
	glm::ivec2 previousPos;
	
};

int main(void){
	Program program;
	
	return EXIT_SUCCESS;
}
