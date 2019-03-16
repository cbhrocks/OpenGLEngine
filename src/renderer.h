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
#include "shader.h"

#define CUBE_TEXTURE_SIZE 256

class Renderer
{
    public:
        int toIntRange = 0;

        Renderer()
        {
            initialized = false;

            //view
            this->width = 640;
            this->height = 480;

            //shader
        }

        Renderer(int width, int height)
        {
            initialized = false;

            //view
            this->width = width;
            this->height = height;

            initialize();
        }

        void setRes(int width, int height){
            glViewport(0, 0, this->width, this->height);
        }

		void preRender(Scene* scene)
		{
			glStencilMask(0xFF);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		void postRender(Scene* scene)
		{

		}

		void renderTexture(Scene* scene)
		{
            scene->draw();
			//scene->drawSkybox();
		}

		void renderHighlight(Scene* scene)
		{
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);

			scene->drawHighlight();

            checkGLError("render highlight");

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

        GLuint squareVertexArray;

        TextBufferManager2D textureMgr;

        int width, height;

        void initialize()
        {
            printf("initializing renderer\n");
            initialized = true;

            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
			//glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
			//glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_REPLACE, GL_REPLACE);

			//glEnable(GL_FRAMEBUFFER_SRGB);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_PROGRAM_POINT_SIZE);
            checkGLError("Renderer::initialize");
        }
};

#endif
