#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <iostream>
#include "model.h"
#include "light.h"
#include "shader.h"

class Camera{
    public:
        glm::vec3 position;
        glm::vec3 look;
        glm::vec3 up;
        float farBound;
        float nearBound;
        float aspectRatio;
        float fov;

        Camera() {
        }

        Camera(glm::vec3 pos, glm::vec3 look, glm::vec3 up, float farBound, float nearBound, float aspectRatio, float fov)
        {
            this->position = pos;
            this->look = look;
            this->up = up;
            this->farBound = farBound;
            this->nearBound = nearBound;
            this->aspectRatio = aspectRatio;
            this->fov = fov;
        }

        glm::mat4 getProjectionMatrix() 
        {
            return glm::perspective(this->fov, this->aspectRatio, this->nearBound, this->farBound); // using aspect ratio
            //return glm::perspectiveFov(1.0f, (float) this->width, (float) this->height, this->near, this->far); // using width and height
        }

        glm::mat4 getViewMatrix() const
        { 
            return glm::lookAt(this->position, this->position + this->look, this->up); 
        }

        void setPos(glm::vec3 pos)
        { this->position = pos; }

        void setPos(glm::vec3 pos, glm::vec3 look, glm::vec3 up)
        {
            this->position = pos;
            this->look = look;
            this->up = up;
        }
};

class Scene{
    public:
        double currentTime;

        glm::vec3 up;

        Camera camera;
        //GLfloat yaw;
        //GLfloat pitch;

        bool modelRotating;

        float center[3];

        //int currentRes[2];

        Scene() {}

        Scene(float aspectRatio) //: 
            //basicLights(1), directionLights(1), pointLights(1), spotLights(1)
        {

            running = true;

            //model = Model(std::string("objects/test/sibenik.obj"));
            model = Model(std::string("objects/test/nanosuit/nanosuit.obj"));
            //model = Model(std::string("objects/inanimate/tiles/hex-sides.obj"));

            up = glm::vec3(0, 1, 0);

            this->camera = Camera(glm::vec3(0,0,3), glm::vec3(0,0,-1), glm::vec3(0,1,0), 100.0f, 0.1f, aspectRatio, 45.0f);

            GLchar const *lightVPath = "src/shaders/light.vert";
            GLchar const *lightFPath = "src/shaders/light.frag";
            lightShader = Shader(lightVPath, lightFPath);
            
            BasicLight* basicLight = new BasicLight(
                    glm::vec3(0.2f, 0.2f, 0.2f), 
                    glm::vec3(0.5f, 0.5f, 0.5f), 
                    glm::vec3(1.0f, 1.0f, 1.0f), 
                    glm::vec3(0.0f, 5.0f, 5.0f)
                    );
            this->basicLights.push_back(basicLight);
            DirectionLight* directionLight = new DirectionLight(
                    glm::vec3(0.2f, 0.2f, 0.2f), 
                    glm::vec3(0.5f, 0.5f, 0.5f), 
                    glm::vec3(1.0f, 1.0f, 1.0f), 
                    glm::vec3(0.0f, -1.0f, 0.0f)
                    );
            this->directionLights.push_back(directionLight);
            PointLight* pointLight = new PointLight(
                    glm::vec3(0.2f, 0.2f, 0.2f), 
                    glm::vec3(0.5f, 0.5f, 0.5f), 
                    glm::vec3(1.0f, 1.0f, 1.0f), 
                    glm::vec3(0.0f, 8.0f, 5.0f),
                    1.0f,
                    0.09f,
                    0.032f
                    );
            this->pointLights.push_back(pointLight);
            SpotLight* spotLight = new SpotLight(
                    glm::vec3(0.2f, 0.2f, 0.2f), 
                    glm::vec3(0.5f, 0.5f, 0.5f), 
                    glm::vec3(1.0f, 1.0f, 1.0f), 
                    //glm::vec3(0.0f, 10.0f, 5.0f),
                    this->camera.position,
                    glm::vec3(0.0f, 0.0f, -1.0f),
                    1.0f,
                    0.14f,
                    0.07f,
                    glm::cos(glm::radians(12.5f)),
                    glm::cos(glm::radians(17.5f))
                    );
            this->spotLights.push_back(spotLight);
        }

        void uploadUniforms(Shader shader) {
            shader.setFloat("near", this->camera.nearBound);
            shader.setFloat("far", this->camera.farBound);
            shader.setFloat("fov", this->camera.fov);
            shader.setVec3("camPos", this->camera.position);

            checkGLError("upload uniforms -- set camera data");

            glm::mat4 projection = this->camera.getProjectionMatrix();
            glm::mat4 view = this->camera.getViewMatrix();
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat3 normal = glm::inverseTranspose(glm::mat3(model));

            shader.setMat4("P", projection);
            shader.setMat4("V", view);
            shader.setMat4("M", model);
            shader.setMat3("N", normal);

            checkGLError("upload uniforms -- matrices");

            float objectShininess = 32.0f;

            shader.setInt("material.diffuse", 0);
            shader.setInt("material.specular", 1);
            shader.setFloat("material.shininess", objectShininess);

            for (int i = 0; i < this->pointLights.size(); i++){
                string lightNum = std::to_string(i);
                this->pointLights[i]->uploadUniforms(shader, lightNum);
            }
            for (int i = 0; i < this->basicLights.size(); i++){
                string lightNum = std::to_string(i);
                this->basicLights[i]->uploadUniforms(shader, lightNum);
            }
            for (int i = 0; i < this->directionLights.size(); i++){
                string lightNum = std::to_string(i);
                this->directionLights[i]->uploadUniforms(shader, lightNum);
            }
            for (int i = 0; i < this->spotLights.size(); i++){
                string lightNum = std::to_string(i);
                this->spotLights[i]->uploadUniforms(shader, lightNum);
            }

            checkGLError("upload uniforms -- light colors");
        }

		void onFrame()
		{
            this->spotLights[0]->setPos(this->camera.position);
            this->spotLights[0]->setDirection(this->camera.look);
			this->pointLights[0]->setPos(glm::vec4(pointLights[0]->GetPos(), 1.0f) * glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		}

        void drawLights()
        {
            lightShader.Use();
            uploadUniforms(lightShader);
            for(int i = 0; i < this->pointLights.size(); i++){
                this->pointLights[i]->Draw(lightShader);
            }
            //for(int i = 0; i < this->basicLights.size(); i++){
            //    this->basicLights[i]->Draw(lightShader);
            //}
            //for(int i = 0; i < this->directionLights.size(); i++){
            //    this->directionLights[i]->Draw(lightShader);
            //}
            //for(int i = 0; i < this->spotLights.size(); i++){
            //    this->spotLights[i]->Draw(lightShader);
            //}
        }

        void drawModels(Shader shader)
        { 
            this->model.Draw(shader);
        }

        void timeStep(double t)
        {
            this->currentTime = t;
			this->onFrame();
        }

        Model const & getModel() const
        { return model; }

        glm::mat4 getViewMatrix() const
        { return this->camera.getViewMatrix(); }

        glm::vec3 getCameraUp() const
        { return this->camera.up; }

        glm::vec3 getCameraPos() const
        { return this->camera.position; }

        glm::vec3 getCameraLook() const
        { return this->camera.look; }

        void setCameraPos(glm::vec3 pos)
        {
            this->camera.setPos(pos);
        }

        void setCameraPos(glm::vec3 pos, glm::vec3 look, glm::vec3 up)
        {
            this->camera.setPos(pos, look, up);
        }

        void setCamView(glm::vec3 look, glm::vec3 up)
        {
            this->camera.look = look;
            this->camera.up = up;
        }

    private:
        Model model;
        bool running;
        Shader lightShader;

        glm::vec3 startPoint;
        glm::vec3 endPoint;
        vector<BasicLight*> basicLights;
        vector<PointLight*> pointLights;
        vector<DirectionLight*> directionLights;
        vector<SpotLight*> spotLights;

};

#endif
