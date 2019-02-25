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

        Renderer(): shader(6)
        {
            initialized = false;

            //view
            this->width = 640;
            this->height = 480;

            //shader
            this->sid = 0;
        }

        Renderer(Scene &scene, int width, int height): shader(6)
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
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xff);

            this->sid = 0;

            shaders["basic"].Use();
            scene.uploadUniforms(shaders["basic"]);
            scene.drawModels(shaders["basic"]);
        }

        void renderLight(Scene &scene)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xff);

            scene.drawLights();
            this->sid = 4;

            shaders["light"].Use();
            scene.uploadUniforms(shaders["light"]);
            scene.drawModels(shaders["light"]);
        }

		void renderHighlight(Scene &scene)
		{
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);

			scene.scaleModels(glm::vec3(1.1f));

			this->shaders["highlight"].Use();
			scene.uploadCameraUniforms(this->shaders["highlight"]);
			scene.uploadModelUniforms(this->shaders["highlight"]);
			scene.drawModels(this->shaders["highlight"]);
			scene.scaleModels(glm::vec3(1.0f));

            checkGLError("render highlight");

			glStencilMask(0xff);
			glEnable(GL_DEPTH_TEST);
		}

        void renderNormals(Scene &scene)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            this->sid = 0;
            shaders["basic"].Use();
            scene.uploadUniforms(shaders["basic"]);
            scene.drawModels(shaders["basic"]);

            this->sid = 2;

            shaders["normal"].Use();
            scene.uploadUniforms(shaders["normal"]);
            scene.drawModels(shaders["normal"]);
            
            checkGLError("render basic");

        }

        void render2D(Scene &scene){
            glBindFramebuffer(GL_FRAMEBUFFER, this->textureMap.framebuffer);
            //glViewport(0, 0, this->width, this->height);

            this->sid = 1;
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            shaders["depth"].Use();
            scene.uploadUniforms(shaders["depth"]);
            scene.drawModels(shaders["depth"]);

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
            shaders["texture2D"].Use();
            scene.uploadUniforms(shaders["texture2D"]);
            shaders["texture2D"].setInt("texId", 0);
            shaders["texture2D"].setInt("toIntRange", toIntRange);

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
		std::map<std::string, Shader> shaders;
        GLuint squareVertexArray;
        size_t sid;

        TextBufferManager2D textureMap;

        int width, height;

        void initialize(Scene &scene)
        {
            printf("initializing renderer\n");
            initialized = true;

            glEnable(GL_DEPTH_TEST);
			glEnable(GL_STENCIL_TEST);
            glCullFace(GL_BACK);
			glDepthFunc(GL_LESS);
            //glEnable(GL_CULL_FACE);

			//stencil test defaults
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

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
			this->shaders["basic"] = Shader(basicVPath, basicFPath);

            GLchar const *depthVPath = "src/shaders/depth.vert";
            GLchar const *depthFPath = "src/shaders/depth.frag";
			this->shaders["depth"] = Shader(depthVPath, depthFPath);

            GLchar const *normalVPath = "src/shaders/normal.vert";
            GLchar const *normalFPath = "src/shaders/normal.frag";
            GLchar const *normalGPath = "src/shaders/normal.geom";
			this->shaders["normal"] = Shader(normalVPath, normalFPath, normalGPath);

            GLchar const *texVPath = "src/shaders/texture2D.vert";
            GLchar const *texFPath = "src/shaders/texture2D.frag";
			this->shaders["texture2D"] = Shader(texVPath, texFPath);

            GLchar const *lightVPath = "src/shaders/lights.vert";
            GLchar const *lightFPath = "src/shaders/lights.frag";
			this->shaders["light"] = Shader(lightVPath, lightFPath);

            GLchar const *highlightVPath = "src/shaders/basic.vert";
            GLchar const *highlightFPath = "src/shaders/highlight.frag";
			this->shaders["highlight"] = Shader(highlightVPath, highlightFPath);

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
};

#endif
