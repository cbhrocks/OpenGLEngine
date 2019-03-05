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

        Renderer(): shader(7)
        {
            initialized = false;

            //view
            this->width = 640;
            this->height = 480;

            //shader
        }

        Renderer(Scene &scene, int width, int height): shader(7)
        {
            initialized = false;

            //view
            this->width = width;
            this->height = height;

            initialize(scene);
        }

        void setRes(int width, int height){
            this->width = width;
            this->height = height;
            glViewport(0, 0, this->width, this->height);
            this->textureMgr.setDimensions(this->width, this->height);
        }

        void render(Scene &scene)
        {
            //printf("using shader %d\n", this->sid);

            //use shader
            //uploadUniforms(scene);
            //scene.uploadUniforms(shader[this->sid]);

            //scene.drawModels(shader[this->sid]);

            checkGLError("render draw");
        }

        void renderBasic(Scene &scene)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xff);

            shaders["basic"].Use();
            scene.drawModels(shaders["basic"]);
        }

		void renderTexture(Scene &scene)
		{
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xff);

			shaders["skybox"].Use();
			scene.drawSkybox(shaders["skybox"]);
            shaders["texture"].Use();
			checkGLError("render texture");
            scene.drawModels(shaders["texture"]);
			shaders["reflection"].Use();
			scene.drawReflections(shaders["reflection"]);
		}

        void renderLight(Scene &scene)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xff);

            scene.drawLights();

            shaders["light"].Use();
            scene.drawModels(shaders["light"]);
        }

		void renderHighlight(Scene &scene)
		{
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);

			scene.scaleModels(glm::vec3(1.1f));

			this->shaders["highlight"].Use();
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

            shaders["basic"].Use();
            scene.drawModels(shaders["basic"]);

            shaders["normal"].Use();
            scene.drawModels(shaders["normal"]);
            
            checkGLError("render basic");
        }

        void renderDepth(Scene &scene){
            //glBindFramebuffer(GL_FRAMEBUFFER, this->textureMap.framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            shaders["depth"].Use();
            scene.drawModels(shaders["depth"]);

			checkGLError("render depth");
        }

        void render2D(Scene &scene){

            //glViewport(0, 0, this->textureMap.width, this->textureMap.height);
            glBindFramebuffer(GL_FRAMEBUFFER, this->textureMgr.FBO);

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xff);

			shaders["light"].Use();
			//scene.drawLights();
			scene.drawModels(shaders["light"]);

            checkGLError("display2D-0");

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			this->textureMgr.Draw();

			glEnable(GL_DEPTH_TEST);
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

        TextBufferManager2D textureMgr;

        int width, height;

        void initialize(Scene &scene)
        {
            printf("initializing renderer\n");
            initialized = true;

            glEnable(GL_DEPTH_TEST);
            //glCullFace(GL_BACK);
			glDepthFunc(GL_LEQUAL);
            glEnable(GL_CULL_FACE);

			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            checkGLError("enable depth test and stuff");

            printf("setting up shader\n");
            createShaderProgs();

            this->textureMgr = TextBufferManager2D(this->width, this->height, this->shaders["basic2D"]);
			this->textureMgr.createVAO();
        }

        //loads different shaders from different files. this could be useful for loading a depth shader vs normal
        void createShaderProgs()
        {
			this->shaders["basic"] = Shader("src/shaders/basic.vert", "src/shaders/basic.frag");
			this->shaders["texture"] = Shader("src/shaders/basic.vert", "src/shaders/texture.frag");
			this->shaders["depth"] = Shader("src/shaders/depth.vert", "src/shaders/depth.frag");
			this->shaders["normal"] = Shader("src/shaders/normal.vert", "src/shaders/normal.frag", "src/shaders/normal.geom");
			this->shaders["basic2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/basic2D.frag");
			this->shaders["inverse2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/inverse2D.frag");
			this->shaders["grey2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/grey2D.frag");
			this->shaders["sharpen2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/sharpen2D.frag");
			this->shaders["blur2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/blur2D.frag");
			this->shaders["edge2D"] = Shader("src/shaders/basic2D.vert", "src/shaders/edge2D.frag");
			this->shaders["skybox"] = Shader("src/shaders/skybox.vert", "src/shaders/skybox.frag");
			this->shaders["light"] = Shader("src/shaders/lights.vert", "src/shaders/lights.frag");
			this->shaders["highlight"] = Shader("src/shaders/basic.vert", "src/shaders/highlight.frag");
			this->shaders["reflection"] = Shader("src/shaders/reflection.vert", "src/shaders/reflection.frag");
            checkGLError("shader");
        }
};

#endif
