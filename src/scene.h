#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <iostream>
#include "model.h"

class Scene{
    public:
        double currentTime;

        glm::vec3 up;

        glm::vec3 cameraPos;
        glm::vec3 cameraLook;
        glm::vec3 cameraUp;
        //GLfloat yaw;
        //GLfloat pitch;

        glm::mat4 modelRotate;
        glm::mat4 modelIncrement;
        glm::mat4 modelTranslate;
        glm::mat4 cameraMatrix;

        bool modelRotating;

        float center[3];
        //int currentRes[2];

        Scene() {

            running = true;

            try{
                GLFWwindow* window = glfwGetCurrentContext();
                cout << "OpenGL Version: " << GLVersion.major << "." << GLVersion.minor << endl;
            }
            catch (const exception& e) {
                std::cout << e.what();
            }

            //if (!glGetError())
            //{
            //    std::cout << "Failed to initialize GLAD in scene" << std::endl;
            //}

            //model = Model(std::string("objects/test/sibenik.obj"));
            model = Model(std::string("objects/inanimate/tiles/hex-sides.obj"));

            //glm::vec3 center = model.getCentroid();
            //glm::vec3 max = model.getMaxBound();
            //glm::vec3 min = model.getMinBound();
            //glm::vec3 dim = model.getDimension();
            //printf("model loaded\n");
            //printf("min [%.2f %.2f %.2f]\n", min[0], min[1], min[2]);
            //printf("max [%.2f %.2f %.2f]\n", max[0], max[1], max[2]);
            //printf("cen [%.2f %.2f %.2f]\n", center[0], center[1], center[2]);
            //printf("dim [%.2f %.2f %.2f]\n", dim[0], dim[1], dim[2]);
            //float camDistance = std::max(dim[0], dim[1]);
            //float camDistance = std::max(dim[0], dim[1]);

            up = glm::vec3(0, 1, 0);

            cameraPos = glm::vec3(0,3,0);
            cameraLook = cameraPos + glm::vec3(-1,0,0);
            cameraUp = cameraPos + glm::vec3(0,1,0);

            modelRotate = glm::mat4(1);
            modelIncrement = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0,1,0));
            //modelTranslate = glm::translate(glm::mat4(1), -model.getCentroid());

            modelRotating = false;
        }

        Model const & getModel() const
        { return model; }

        void drawModels(Shader shader)
        { this->model.Draw(shader);  }

        //void setRunning(bool r)
        //{ running = r; }

        //bool isRunning() const
        //{ return running; }

        void timeStep(double t)
        {
            //spin model
            if(modelRotating)
                modelRotate = modelIncrement * modelRotate;

            this->currentTime = t;
        }

        //Model & getModel()
        //{ return model; }

        //glm::mat4 getModelTranslate() const
        //{ return modelTranslate; }

        glm::mat4 getModelRotate() const
        { return modelRotate; }

        glm::mat4 getCameraMatrix() const
        { return glm::lookAt(cameraPos, cameraPos + cameraLook, cameraUp); }

        glm::vec4 getCameraPos() const
        { return glm::vec4(this->cameraPos, 1); }

        glm::vec3 getCameraLook() const
        { return glm::vec3(this->cameraLook); }

        //void toggleModelRotate()
        //{ modelRotating = !modelRotating; }

        void setCamView(glm::vec3 look, glm::vec3 up)
        {
            cameraLook = look;
            cameraUp = up;
        }

    private:
        Model model;
        bool running;

        glm::vec3 startPoint;
        glm::vec3 endPoint;

};

#endif
