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

        bool modelRotating;

        float center[3];
        //int currentRes[2];

        Scene() {

            running = true;

            //model = Model(std::string("objects/test/sibenik.obj"));
            model = Model(std::string("objects/test/nanosuit/nanosuit.obj"));
            //model = Model(std::string("objects/inanimate/tiles/hex-sides.obj"));

            up = glm::vec3(0, 1, 0);
            cameraPos = glm::vec3(0,3,0);
            cameraLook = cameraPos + glm::vec3(-1,0,0);
            cameraUp = cameraPos + glm::vec3(0,1,0);

            modelRotating = false;
        }

        Model const & getModel() const
        { return model; }

        void drawModels(Shader shader)
        { this->model.Draw(shader);  }

        void timeStep(double t)
        {
            this->currentTime = t;
        }

        glm::mat4 getViewMatrix() const
        { return glm::lookAt(cameraPos, cameraPos + cameraLook, cameraUp); }

        glm::vec4 getCameraPos() const
        { return glm::vec4(this->cameraPos, 1); }

        glm::vec3 getCameraLook() const
        { return glm::vec3(this->cameraLook); }

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
