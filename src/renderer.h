#ifndef __RENDERER
#define __RENDERER

#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glHelper.h"
#include "scene.h"

#define CUBE_TEXTURE_SIZE 256

class Renderer
{
    public:
        int toIntRange = 0;

        Renderer()
        {
            initialized = false;

            //camera
            this->P = glm::perspective(1.0f, 1.0f, 0.1f, 100.0f);
            this->C = glm::mat4(1);
            this->M = glm::mat4(1);
        }

        Renderer(Scene &scene)
        {
            initialized = false;

            //camera
            this->P = glm::perspective(1.0f, 1.0f, 0.1f, 100.0f);
            this->C = glm::mat4(1);
            this->M = glm::mat4(1);

            initialize(scene);
        }

        void render(Scene &scene)
        {
            checkGLError("model0");

            glClearColor(1.0f, 0.5f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            C = scene.getCameraMatrix();

            //use shader
            glUseProgram(shaderProg[sid]);
            uploadUniforms(shaderProg[sid], scene);
            checkGLError("model1");

            //draw
            glBindVertexArray(vertexArray);
            glDrawElements(GL_TRIANGLES, scene.getModel().getElements().size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            checkGLError("model2");
        }

    private:
        bool initialized;
        GLuint shaderProg[5];
        GLuint vertexArray;
        GLuint squareVertexArray;
        GLuint frameBuffer;
        GLuint renderTexture;
        GLuint renderBuffer;
        size_t sid;

        glm::vec3 lookVecs[6] =
        {
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, -1.0f)
        };

        glm::vec3 upVecs[6] =
        {
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)
        };

        GLuint cFrameBuffer;
        GLuint projectionUniformBuffer;

        glm::mat4 P; //perspective
        glm::mat4 C; //camera
        glm::mat4 M; //model

        glm::vec2 size;

        float _near;
        float _far;
        float fov;
        int totalLevel;
        int face;


        void initialize(Scene & scene)
        {
            printf("initializing renderer\n");
            initialized = true;

            this->face = 4;

            glEnable(GL_DEPTH_TEST);
            glCullFace(GL_BACK);
            glEnable(GL_CULL_FACE);
            checkGLError("enable depth test and stuff");

            glm::vec3 dim = scene.getModel().getDimension();
            float maxDim = std::max(dim[0], std::max(dim[1], dim[2]));
            _near = 0.01f;
            //_near = 1.0f;
            _far = maxDim*3;
            //_far = 100.0f;
            fov = 1.5708f;
            this->P = glm::perspective(fov, 1.0f, _near, _far);
            C = scene.getCameraMatrix();

            printf("setting up shader\n");
            setupShader();
            setupBuffers(scene.getModel());
        }

        //loads different shaders from different files. this could be useful for loading a depth shader vs normal
        void setupShader()
        {
            char const * texVPath = "resources/texture2D.vert";
            char const * texFPath = "resources/texture2D.frag";
            shaderProg[0] = ShaderManager::shaderFromFile(&texVPath, &texFPath, 1, 1);

            char const * depthVPath = "resources/depth.vert";
            char const * depthFPath = "resources/depth.frag";
            shaderProg[1] = ShaderManager::shaderFromFile(&depthVPath, &depthFPath, 1, 1);

            checkGLError("shader");
        }

        void setupBuffers(Model & model)
        {
            glGenVertexArrays(1, &vertexArray);
            glBindVertexArray(vertexArray);

            GLuint positionBuffer;
            GLuint colorBuffer;
            GLuint elementBuffer;
            GLint colorSlot;
            GLint positionSlot;

            //setup position buffer
            glGenBuffers(1, &positionBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
            glBufferData(GL_ARRAY_BUFFER, model.getPositionBytes(), &model.getPosition()[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
            positionSlot = glGetAttribLocation(shaderProg[0], "pos");
            glEnableVertexAttribArray(positionSlot);
            glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            checkGLError("setup position buffer");

            // now the elements
            //
            // element buffer is the buffer that is used to designate which
            // vertices from the position buffer are used to make the triangles
            glGenBuffers(1, &elementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.getElementBytes(), &model.getElements()[0], GL_STATIC_DRAW);
            checkGLError("setup element buffer");
            //leave the element buffer active


            glEnableVertexAttribArray(glGetAttribLocation(shaderProg[3], "pos"));
            glVertexAttribPointer(glGetAttribLocation(shaderProg[3], "pos"), 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);

            checkGLError("setup render to texture##");

            static const GLfloat squareVertexData[] = {
                -1.0f, -1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f,

                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                1.0f, 1.0f, 0.0f
            };

            glGenVertexArrays(1, &squareVertexArray);
            glBindVertexArray(squareVertexArray);
            GLuint squareVertexBuffer;
            glGenBuffers(1, &squareVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, squareVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertexData), &squareVertexData, GL_STATIC_DRAW);

            glEnableVertexAttribArray(glGetAttribLocation(shaderProg[4], "pos"));
            glVertexAttribPointer(glGetAttribLocation(shaderProg[4], "pos"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

            checkGLError("setup render to texture");
        }

        void uploadUniforms(GLuint shaderId, Scene const & scene)
        {
            glm::mat4 mT = scene.getModelTranslate();
            glm::mat4 mR = scene.getModelRotate();
            glm::mat4 M = C;//*mR*mT; dont want to do anything to camera
            glm::mat3 N = glm::inverseTranspose(glm::mat3(M));
            glm::vec4 camPos = scene.getCameraPos();

            glUniform1f(glGetUniformLocation(shaderId, "near"), _near);
            glUniform1f(glGetUniformLocation(shaderId, "far"), _far);
            glUniform1f(glGetUniformLocation(shaderId, "fov"), fov);
            glUniform1i(glGetUniformLocation(shaderId, "toIntRange"), toIntRange);

            glUniformMatrix4fv(glGetUniformLocation(shaderId, "P"), 1, GL_FALSE, &P[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "C"), 1, GL_FALSE, &C[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "mR"), 1, GL_FALSE, &mR[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "mT"), 1, GL_FALSE, &mT[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "M"), 1, GL_FALSE, &M[0][0]);
            glUniformMatrix3fv(glGetUniformLocation(shaderId, "N"), 1, GL_FALSE, &N[0][0]);
            glUniform4fv(glGetUniformLocation(shaderId, "camPos"), 1, &camPos[0]);
        }
};

#endif
