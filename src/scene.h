#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <iostream>
#include "model.h"
#include "light.h"
#include "shader.h"

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
            cameraPos = glm::vec3(0,0,3);
            cameraLook = glm::vec3(0,0,-1);
            cameraUp = glm::vec3(0,1,0);

            GLchar const *lightVPath = "src/shaders/light.vert";
            GLchar const *lightFPath = "src/shaders/light.frag";
            lightShader = Shader(lightVPath, lightFPath);
            
            Light light = Light(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f));
            this->lights.push_back(light);
        }

        Model const & getModel() const
        { return model; }

        void drawModels(Shader shader)
        { 
            this->model.Draw(shader);  
            lightShader.Use();
            glm::mat4 ViewMat = getViewMatrix();
            glm::mat4 ModelMat = glm::mat4(1.0f);
            glm::mat4 ProjectionMat = glm::perspectiveFov(1.0f, 640.0f, 480.0f, 0.1f, 100.0f); // using width and height
            lightShader.setMat4("V", ViewMat);
            lightShader.setMat4("M", ModelMat);
            lightShader.setMat4("P", ProjectionMat);
            for(int i = 0; i < lights.size(); i++){
                this->lights[i].Draw(lightShader);
            }
        }

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

        glm::vec3 getLightPos() const
        { return this->lights[0].position; }

        glm::vec3 getLightAmbient() const
        { return this->lights[0].ambient; }

        glm::vec3 getLightDiffuse() const
        { return this->lights[0].diffuse; }

        glm::vec3 getLightSpecular() const
        { return this->lights[0].specular; }

        void setCamView(glm::vec3 look, glm::vec3 up)
        {
            cameraLook = look;
            cameraUp = up;
        }

    private:
        Model model;
        bool running;
        Shader lightShader;

        glm::vec3 startPoint;
        glm::vec3 endPoint;
        vector<Light> lights;

};

#endif
