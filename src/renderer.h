#ifndef __RENDERER
#define __RENDERER

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

        Renderer(): shaderProg(3), shader(3)
        {
            initialized = false;

            //view
            this->width = 640;
            this->height = 480;

            //shader
            this->sid = 0;
        }

        Renderer(Scene &scene, int width, int height): shaderProg(3), shader(3)
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
            uploadUniforms(shader[this->sid].getId(), scene);
            checkGLError("render shader");

            //draw
            glBindVertexArray(this->vertexArray);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);
            glDrawElements(GL_TRIANGLES, scene.getModel().getPositions().size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            checkGLError("render draw");
        }

        void renderNormals(Scene &scene)
        {
            //use shader
            shader[this->sid].Use();
            uploadUniforms(shader[this->sid].getId(), scene);
            checkGLError("render normals shader");

            //draw
            glBindVertexArray(this->vertexArray);
            //glBindBuffer(GL_ARRAY_BUFFER, this->vertexNormalbuffer);
            glDrawArrays(GL_POINTS, 0, scene.getModel().getPositions().size());
            glBindVertexArray(0);

            checkGLError("render normals draw");
        }

        void renderBasic(Scene &scene)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            this->sid = 0;
            render(scene);

            this->sid = 2;
            renderNormals(scene);
        }

        void render2D(Scene &scene){
            glBindFramebuffer(GL_FRAMEBUFFER, this->textureMap.framebuffer);
            glViewport(0, 0, this->width, this->height);

            this->sid = 1;
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            render(scene);
        }

        void display2D(Scene &scene){
            toIntRange = 1;

            //get old camera data
            glm::vec3 oldUp = scene.cameraUp;
            glm::vec3 oldPos = scene.cameraPos;
            glm::vec3 oldLook = scene.cameraLook;

            //set camera position to in front of 2D texture
            scene.cameraPos = glm::vec3(0,0,0);

            //render using 2D renderer
            this->sid = 3;
            //glUseProgram(shaderProg[this->sid]);
            //uploadUniforms(shaderProg[this->sid], scene);
            shader[this->sid].Use();
            uploadUniforms(shader[this->sid].getId(), scene);

            glViewport(0, 0, this->textureMap.width, this->textureMap.height);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(1.0f, 0.5f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            checkGLError("display2D-1");

            GLuint texId = 0;
            glActiveTexture(GL_TEXTURE0+texId);
            glUniform1i(glGetUniformLocation(shader[this->sid].getId(), "texId"), texId);

            //draw a quad that fills the entire view
            glBindVertexArray(squareVertexArray); //this one
            glDisable(GL_DEPTH_TEST);
            glBindTexture(GL_TEXTURE_2D, this->textureMap.texture);
            glGenerateMipmap(GL_TEXTURE_2D);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            //reset camera data
            scene.cameraUp = oldUp;
            scene.cameraPos = oldPos;
            scene.cameraLook = oldLook;

            checkGLError("display2D-2");
        }

    private:
        bool initialized;

        std::vector<GLuint> shaderProg;
        std::vector<Shader> shader;
        GLuint vertexArray;
        GLuint vertexNormalArray;
        GLuint vertexBuffer;
        GLuint squareVertexArray;
        GLuint elementBuffer;
        GLuint vertexNormalbuffer;
        size_t sid;

        TextBufferManager2D textureMap;

        int width, height;

        void initialize(Scene &scene)
        {
            printf("initializing renderer\n");
            initialized = true;

            glEnable(GL_DEPTH_TEST);
            glCullFace(GL_BACK);
            glEnable(GL_CULL_FACE);
            checkGLError("enable depth test and stuff");

            printf("setting up shader\n");
            createShaderProgs();
            setupBuffers(scene.getModel());

            //textureMap = TextBufferManager2D(scene.currentRes[0], scene.currentRes[1]);
            this->textureMap = TextBufferManager2D(this->width, this->height);
        }

        //loads different shaders from different files. this could be useful for loading a depth shader vs normal
        void createShaderProgs()
        {
            GLchar const *basicVPath = "src/shaders/basic.vert";
            GLchar const *basicFPath = "src/shaders/basic.frag";
            //this->shaderProg[0] = ShaderManager::shaderFromFile(&basicVPath, &basicFPath, 1, 1);
            this->shader[0] = Shader(basicVPath, basicFPath);

            GLchar const *depthVPath = "src/shaders/depth.vert";
            GLchar const *depthFPath = "src/shaders/depth.frag";
            //this->shaderProg[1] = ShaderManager::shaderFromFile(&depthVPath, &depthFPath, 1, 1);
            this->shader[1] = Shader(depthVPath, depthFPath);

            GLchar const *normalVPath = "src/shaders/normal.vert";
            GLchar const *normalFPath = "src/shaders/normal.frag";
            GLchar const *normalGPath = "src/shaders/normal.geom";
            //this->shaderProg[1] = ShaderManager::shaderFromFile(&depthVPath, &depthFPath, 1, 1);
            this->shader[2] = Shader(normalVPath, normalFPath, normalGPath);

            GLchar const *texVPath = "src/shaders/texture2D.vert";
            GLchar const *texFPath = "src/shaders/texture2D.frag";
            //this->shaderProg[2] = ShaderManager::shaderFromFile(&texVPath, &texFPath, 1, 1);
            this->shader[3] = Shader(texVPath, texFPath);

            checkGLError("shader");
        }

        void addShaderAttrib(const GLint location){
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        }

        void setupBuffers(Model &model)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            //gen buffers
            glGenVertexArrays(1, &(this->vertexArray));
            glGenVertexArrays(1, &(this->vertexNormalArray));
            glGenBuffers(1, &(this->vertexBuffer));
            glGenBuffers(1, &(this->elementBuffer));
            //glGenBuffers(1, &(this->vertexNormalbuffer));

            glBindVertexArray(this->vertexArray);

            checkGLError("gen buffers");

            // element buffer is the buffer that is used to designate which
            // vertices from the position buffer are used to make the triangles
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.getElementBytes(), &model.getElements()[0], GL_STATIC_DRAW);
            checkGLError("setup element buffer");

            //setup position buffer and load into shaders
            glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, model.getPositionBytes(), &model.getPositions()[0], GL_STATIC_DRAW);
            checkGLError("setup position buffer0");
            for (size_t i = 0; i < this->shader.size(); i++){
                if (i != 3){
                    this->sid = i;
                    addShaderAttrib(0);
                }
            }

            checkGLError("setup position buffer");

            glBindVertexArray(this->vertexArray);

            //setup normal buffer and load into shaders
            glBindBuffer(GL_ARRAY_BUFFER, this->vertexNormalbuffer);
            glBufferData(GL_ARRAY_BUFFER, model.getVertexNormalBytes(), &model.getVertexNormals()[0], GL_STATIC_DRAW);
            this->sid = 2;
            addShaderAttrib(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            checkGLError("setup normal buffer");

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

            this->sid = 3;
            addShaderAttrib(0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindVertexArray(0);
            checkGLError("setupBuffers End");
        }

        void uploadUniforms(GLuint shaderId, Scene const & scene)
        {
            glm::vec3 dim = scene.getModel().getDimension();
            float maxDim = std::max(dim[0], std::max(dim[1], dim[2]));
            float _near = 0.01f;
            //_near = 1.0f;
            float _far = maxDim*3;
            //_far = 100.0f;
            float fov = 1.5708f;

            glm::mat4 CameraMat = scene.getCameraMatrix();
            glm::mat4 ModelMat = glm::mat4(1.0f);//CameraMat;//*mR*mT; dont want to do anything to camera
            glm::mat4 PerspectiveMat;
            //PerspectiveMat = glm::perspective(fov, 1.0f, _near, _far); // using aspect ratio
            PerspectiveMat = glm::perspectiveFov(1.0f, (float) this->width, (float) this->height, _near, _far); // using width and height
            glm::mat4 ModelMatTranslate = scene.getModelTranslate();
            glm::mat4 ModelMatRotate = scene.getModelRotate();
            glm::mat3 N = glm::inverseTranspose(glm::mat3(ModelMat));
            glm::vec4 camPos = scene.getCameraPos();

            glUniform1f(glGetUniformLocation(shaderId, "near"), _near);
            glUniform1f(glGetUniformLocation(shaderId, "far"), _far);
            glUniform1f(glGetUniformLocation(shaderId, "fov"), fov);
            glUniform1i(glGetUniformLocation(shaderId, "toIntRange"), toIntRange);

            glUniformMatrix4fv(glGetUniformLocation(shaderId, "P"), 1, GL_FALSE, &PerspectiveMat[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "C"), 1, GL_FALSE, &CameraMat[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "mR"), 1, GL_FALSE, &ModelMatRotate[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "mT"), 1, GL_FALSE, &ModelMatTranslate[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderId, "M"), 1, GL_FALSE, &ModelMat[0][0]);
            glUniformMatrix3fv(glGetUniformLocation(shaderId, "N"), 1, GL_FALSE, &N[0][0]);
            glUniform4fv(glGetUniformLocation(shaderId, "camPos"), 1, &camPos[0]);
        }
};

#endif
