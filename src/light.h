#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "shader.h"
#include "glHelper.h"

GLfloat lightVertexData[] {
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
};

class Light 
{
    public:
        /*  Model Data */
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 position;
        float ambientStrength;
        float specularStrength;
        GLuint VAO;

        Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
            this->ambient = ambient;
            this->diffuse = diffuse;
            this->specular = specular;
            this->position = glm::vec3(0.0f, 0.0f, 0.0f);
            setupLight();
        }

        Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position) {
            this->ambient = ambient;
            this->diffuse = diffuse;
            this->specular = specular;
            this->position = position;
            setupLight();
        }

        // draws the model, and thus all its meshes
        void Draw(Shader shader)
        {
            glm::mat4 ModelMat = glm::mat4(1.0f);
            ModelMat = glm::translate(ModelMat, this->position);
            shader.setMat4("M", ModelMat);

            shader.setVec3("light.ambient", this->ambient);
            shader.setVec3("light.diffuse", this->diffuse);
            shader.setVec3("light.specular", this->specular);
            glEnable(GL_DEPTH_TEST);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

    private:
        GLuint VBO;

        void setupLight()
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertexData), lightVertexData, GL_STATIC_DRAW);

            glBindVertexArray(VAO);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);
        }
};

#endif

