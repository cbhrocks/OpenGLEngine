#ifndef __RENDERER
#define __RENDERER

#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glHelper.h"
#include "scene.h"
#include "textBufferManager2D.h"

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

            this->sid = 0;
        }

        Renderer(Scene &scene)
        {
            initialized = false;

            //camera
            this->P = glm::perspective(1.0f, 1.0f, 0.1f, 100.0f);
            this->C = glm::mat4(1);
            this->M = glm::mat4(1);

            this->sid = 0;

            initialize(scene);
        }

        void render(Scene &scene)
        {
            checkGLError("model0");

            glClearColor(1.0f, 0.5f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            C = scene.getCameraMatrix();

            printf("using shader %d\n", this->sid);

            //use shader
            glUseProgram(shaderProg[this->sid]);
            uploadUniforms(shaderProg[this->sid], scene);
            checkGLError("model1");

            //draw
            glBindVertexArray(vertexArray);
            glDrawElements(GL_TRIANGLES, scene.getModel().getElements().size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            checkGLError("model2");
        }

        void render2D(Scene &scene){
            glViewport(0, 0, textureMap.width, textureMap.height);
            glBindFramebuffer(GL_FRAMEBUFFER, textureMap.framebuffer);

            this->sid = 0;
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_2D, textureMap.texture, 0);

            glClear(GL_DEPTH_BUFFER_BIT);

            checkGLError("render textureMap-0");

            render(scene);

            glBindTexture(GL_TEXTURE_2D, textureMap.texture);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            checkGLError("render textureMap-end");
        }

        void display2D(Scene &scene){
            glViewport(0, 0, textureMap.width, textureMap.height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);

            //get old camera data
            glm::vec3 oldUp = scene.cameraUp;
            glm::vec3 oldPos = scene.cameraPos;
            glm::vec3 oldLook = scene.cameraLook;

            //set camera position to in front of 2D texture
            scene.cameraPos = glm::vec3(0,0,0);
            scene.setCamView(lookVecs[face], upVecs[face]);
            C = scene.getCameraMatrix();

            //render using 2D renderer
            this->sid = 1;
            glUseProgram(shaderProg[this->sid]);
            uploadUniforms(shaderProg[this->sid], scene);
            //glUniform1i(glGetUniformLocation(shaderProg[this->sid], "level"), 20);

            checkGLError("display2D-1");

            //draw a quad that fills the entire view
            GLuint texId = 0;
            glActiveTexture(GL_TEXTURE0+texId);
            glUniform1i(glGetUniformLocation(shaderProg[this->sid], "texId"), texId);
            glBindTexture(GL_TEXTURE_2D, textureMap.texture);

            glBindVertexArray(squareVertexArray); //this one

            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glBindVertexArray(0);
            glUseProgram(0);

            //reset camera data
            scene.cameraUp = oldUp;
            scene.cameraPos = oldPos;
            scene.cameraLook = oldLook;

            checkGLError("display2D-2");
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

        TextBufferManager2D textureMap;

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


        void initialize(Scene &scene)
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

            textureMap = TextBufferManager2D(scene.currentRes[0], scene.currentRes[1]);
        }

        //loads different shaders from different files. this could be useful for loading a depth shader vs normal
        void setupShader()
        {
            char const * depthVPath = "src/shaders/depth.vert";
            char const * depthFPath = "src/shaders/depth.frag";
            shaderProg[0] = ShaderManager::shaderFromFile(&depthVPath, &depthFPath, 1, 1);

            char const * texVPath = "src/shaders/texture2D.vert";
            char const * texFPath = "src/shaders/texture2D.frag";
            shaderProg[1] = ShaderManager::shaderFromFile(&texVPath, &texFPath, 1, 1);

            checkGLError("shader");
        }

        void setupBuffers(Model &model)
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


            //glEnableVertexAttribArray(glGetAttribLocation(shaderProg[3], "pos"));
            //glVertexAttribPointer(glGetAttribLocation(shaderProg[3], "pos"), 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);

            checkGLError("setup render to texture");

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

            glEnableVertexAttribArray(glGetAttribLocation(shaderProg[1], "pos"));
            glVertexAttribPointer(glGetAttribLocation(shaderProg[1], "pos"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

            checkGLError("setupBuffers End");
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
