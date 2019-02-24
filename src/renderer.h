#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glHelper.h"
#include "scene.h"
#include "textBufferManager2D.h"
#include "shaderManager.h"
#include "shader.h"

#define CUBE_TEXTURE_SIZE 256

class Renderer
{
    public:
        int toIntRange = 0;

        Renderer(): shader(5)
        {
            initialized = false;

            //view
            this->width = 640;
            this->height = 480;

            //shader
            this->sid = 0;
        }

        Renderer(Scene &scene, int width, int height): shader(5)
        {
            initialized = false;

            //view
            this->width = width;
            this->height = height;

            this->sid = 0;
            initialize(scene);
        }

        void setRes(int width, int height){
            this->width = width;
            this->height = height;
            glViewport(0, 0, this->width, this->height);
            this->textureMap = TextBufferManager2D(this->width, this->height);
        }

        void render(Scene &scene)
        {
            //printf("using shader %d\n", this->sid);

            //use shader
            shader[this->sid].Use();
            //uploadUniforms(scene);
            scene.uploadUniforms(shader[this->sid]);

            scene.drawModels(shader[this->sid]);

            checkGLError("render draw");
        }

        void renderBasic(Scene &scene)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            this->sid = 0;
            render(scene);
        }

        void renderLight(Scene &scene)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            scene.drawLights();
            this->sid = 4;
            render(scene);
        }

        void renderNormals(Scene &scene)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            this->sid = 0;
            render(scene);

            this->sid = 2;
            render(scene);
            
            checkGLError("render basic");

        }

        void render2D(Scene &scene){
            glBindFramebuffer(GL_FRAMEBUFFER, this->textureMap.framebuffer);
            //glViewport(0, 0, this->width, this->height);

            this->sid = 1;
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            render(scene);

            display2D(scene);
        }

        void display2D(Scene &scene){
            toIntRange = 1;

            //get old camera data
            glm::vec3 oldPos = scene.getCameraPos();
            glm::vec3 oldLook = scene.getCameraLook();
            glm::vec3 oldUp = scene.getCameraUp();
            glm::vec3 displayPos = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 displayLook = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 displayUp = glm::vec3(0.0f, 1.0f, 0.0f);

            //set camera position to in front of 2D texture

            scene.setCameraPos(displayPos, displayLook, displayUp);
            //scene.cameraPos = glm::vec3(0,0,0);

            //glViewport(0, 0, this->textureMap.width, this->textureMap.height);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(1.0f, 0.5f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            checkGLError("display2D-0");

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->textureMap.texture);

            checkGLError("display2D-1");

            //upload uniforms
            this->sid = 3;
            shader[this->sid].Use();
            scene.uploadUniforms(shader[this->sid]);
            shader[this->sid].setInt("texId", 0);
            shader[this->sid].setInt("toIntRange", toIntRange);

            checkGLError("display2D-2");

            //draw a quad that fills the entire view
            glDisable(GL_DEPTH_TEST);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindVertexArray(squareVertexArray); //this one
            glDrawArrays(GL_TRIANGLES, 0, 6);

            checkGLError("display2D-3");

            //reset camera data
            scene.setCameraPos(oldPos, oldLook, oldUp);
            //scene.cameraUp = oldUp;
            //scene.cameraPos = oldPos;
            //scene.cameraLook = oldLook;

            checkGLError("display2D-4");
        }

        void toggleWireframeMode()
        {
            GLint polyMode;
            glGetIntegerv(GL_POLYGON_MODE, &polyMode);

            if ( polyMode == GL_LINE )
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            if ( polyMode == GL_FILL )
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            if ( polyMode == GL_POINTS )
                ;//
        }

    private:
        bool initialized;

        std::vector<Shader> shader;
        GLuint squareVertexArray;
        size_t sid;

        TextBufferManager2D textureMap;

        int width, height;

        void initialize(Scene &scene)
        {
            printf("initializing renderer\n");
            initialized = true;

            glEnable(GL_DEPTH_TEST);
            glCullFace(GL_BACK);
            //glEnable(GL_CULL_FACE);
            checkGLError("enable depth test and stuff");

            printf("setting up shader\n");
            createShaderProgs();
            createTexRender();

            this->textureMap = TextBufferManager2D(this->width, this->height);
        }

        //loads different shaders from different files. this could be useful for loading a depth shader vs normal
        void createShaderProgs()
        {
            GLchar const *basicVPath = "src/shaders/basic.vert";
            GLchar const *basicFPath = "src/shaders/basic.frag";
            this->shader[0] = Shader(basicVPath, basicFPath);

            GLchar const *depthVPath = "src/shaders/depth.vert";
            GLchar const *depthFPath = "src/shaders/depth.frag";
            this->shader[1] = Shader(depthVPath, depthFPath);

            GLchar const *normalVPath = "src/shaders/normal.vert";
            GLchar const *normalFPath = "src/shaders/normal.frag";
            GLchar const *normalGPath = "src/shaders/normal.geom";
            this->shader[2] = Shader(normalVPath, normalFPath, normalGPath);

            GLchar const *texVPath = "src/shaders/texture2D.vert";
            GLchar const *texFPath = "src/shaders/texture2D.frag";
            this->shader[3] = Shader(texVPath, texFPath);

            GLchar const *lightVPath = "src/shaders/lights.vert";
            GLchar const *lightFPath = "src/shaders/lights.frag";
            this->shader[4] = Shader(lightVPath, lightFPath);

            checkGLError("shader");
        }

        void addShaderAttrib(const GLint location){
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        }

        void createTexRender()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glGenVertexArrays(1, &squareVertexArray);
            glBindVertexArray(squareVertexArray);

            static const GLfloat squareVertexData[] = {
                -1.0f, -1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                1.0f, 1.0f, 0.0f
            };

            GLuint squareVertexBuffer;
            glGenBuffers(1, &squareVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, squareVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertexData), &squareVertexData, GL_STATIC_DRAW);

            addShaderAttrib(0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindVertexArray(0);
            checkGLError("setupBuffers End");
        }

        //void uploadUniforms(Scene const & scene)
        //{
        //    //glm::vec3 dim = scene.getModel().getDimension();
        //    //float maxDim = std::max(dim[0], std::max(dim[1], dim[2]));
        //    //float _far = maxDim*3;
        //    checkGLError("upload uniforms");

        //    glm::mat4 ViewMat = scene.getViewMatrix();
        //    glm::mat4 ModelMat = glm::mat4(1.0f);
        //    //glm::mat4 ModelMatTranslate = scene.getModelTranslate();
        //    glm::mat3 N = glm::inverseTranspose(glm::mat3(ModelMat));
        //    glm::vec3 camPos = scene.getCameraPos();

        //    checkGLError("upload uniforms -- create matrices");

        //    shader[this->sid].setFloat("near", this->near);
        //    shader[this->sid].setFloat("far", this->far);
        //    shader[this->sid].setFloat("fov", this->fov);
        //    shader[this->sid].setInt("toIntRange", toIntRange);
        //    shader[this->sid].setVec3("camPos", camPos);

        //    checkGLError("upload uniforms -- set camera data");

        //    shader[this->sid].setMat4("P", this->ProjectionMat);
        //    shader[this->sid].setMat4("V", ViewMat);
        //    shader[this->sid].setMat4("M", ModelMat);
        //    shader[this->sid].setMat3("N", N);

        //    checkGLError("upload uniforms -- matrices");

        //    float objectShininess = 32.0f;

        //    glm::vec3 lightPosition = scene.getLightPos();
        //    glm::vec3 lightAmbient = scene.getLightAmbient();
        //    glm::vec3 lightDiffuse = scene.getLightDiffuse();
        //    glm::vec3 lightSpecular = scene.getLightSpecular();

        //    shader[this->sid].setInt("material.diffuse", 0);
        //    shader[this->sid].setInt("material.specular", 1);
        //    shader[this->sid].setFloat("material.shininess", objectShininess);
        //    shader[this->sid].setVec3("light.position", lightPosition);
        //    shader[this->sid].setVec3("light.ambient", lightAmbient);
        //    shader[this->sid].setVec3("light.diffuse", lightDiffuse);
        //    shader[this->sid].setVec3("light.specular", lightSpecular);

        //    checkGLError("upload uniforms -- light colors");
        //}

};

#endif
