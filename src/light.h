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
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,

    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,

    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
};

class Light 
{
    public:
        /*  Model Data */
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        std::string prefix;

        Light(
                const glm::vec3& ambient, 
                const glm::vec3& diffuse, 
                const glm::vec3& specular, 
                const std::string& prefix = std::string()
             ) : 
            ambient(ambient), 
            diffuse(diffuse), 
            specular(specular), 
            prefix(prefix)
        {
        }

        // draws the model, and thus all its meshes
        virtual void Draw(const Shader& shader) const
        {
        }

        virtual void uploadUniforms(const Shader& shader, const std::string& lightNum) const
        {
            shader.setVec3((this->prefix + "light[" + lightNum + "].ambient").c_str(), this->ambient);
            shader.setVec3((this->prefix + "light[" + lightNum + "].diffuse").c_str(), this->diffuse);
            shader.setVec3((this->prefix + "light[" + lightNum + "].specular").c_str(), this->specular);
        }

    private:
};

class BasicLight : public Light
{
    public:
        GLuint VAO;
        glm::vec3 position;

        BasicLight(
                const glm::vec3& ambient, 
                const glm::vec3& diffuse, 
                const glm::vec3& specular, 
                const glm::vec3& position, 
                const std::string& prefix = std::string("b")
                ) : 
            Light(ambient, diffuse, specular, prefix), 
            position(position)
        {
            setupLight();
        }

        void Draw(Shader shader) const
        {
            glm::mat4 ModelMat = glm::mat4(1.0f);
			ModelMat = glm::scale(glm::translate(ModelMat, this->position), glm::vec3(0.25, 0.25, 0.25));
            shader.setMat4("M", ModelMat);

            shader.setVec3("light.ambient", this->ambient);
            shader.setVec3("light.diffuse", this->diffuse);
            shader.setVec3("light.specular", this->specular);
            glEnable(GL_DEPTH_TEST);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        virtual void uploadUniforms(Shader shader, const std::string& lightNum) const
        {
            Light::uploadUniforms(shader, lightNum);
            shader.setVec3((this->prefix + "light[" + lightNum + "].position").c_str(), this->position);
        }

        virtual glm::vec3 GetPos() const 
        { return this->position; }

        virtual void setPos(glm::vec3 position)
        { this->position = position; }

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


class PointLight : public BasicLight
{
    public:
        float constant;
        float linear;
        float quadratic;

        PointLight(
                const glm::vec3& ambient, 
                const glm::vec3& diffuse, 
                const glm::vec3& specular, 
                const glm::vec3& position, 
                float constant, 
                float linear, 
                float quadratic, 
                const std::string& prefix = std::string("p")
                ) : 
            BasicLight(ambient, diffuse, specular, position, prefix),
            constant(constant), linear(linear), quadratic(quadratic)
        {
        }

        virtual void uploadUniforms(const Shader& shader, const std::string& lightNum) const
        {
            BasicLight::uploadUniforms(shader, lightNum);
            shader.setFloat((this->prefix + "light[" + lightNum + "].constant").c_str(), this->constant);
            shader.setFloat((this->prefix + "light[" + lightNum + "].linear").c_str(), this->linear);
            shader.setFloat((this->prefix + "light[" + lightNum + "].quadratic").c_str(), this->quadratic);
        }
};

class DirectionLight : public Light
{
    public:
        glm::vec3 direction;

        DirectionLight(
                const glm::vec3& ambient, 
                const glm::vec3& diffuse, 
                const glm::vec3& specular, 
                const glm::vec3& direction, 
                const std::string& prefix = std::string("d")
                ) : 
            Light(ambient, diffuse, specular, prefix), 
            direction(direction)
        {
        }

        virtual void uploadUniforms(Shader shader, const std::string& lightNum) const
        {
            Light::uploadUniforms(shader, lightNum);
            shader.setVec3((this->prefix + "light[" + lightNum + "].direction").c_str(), this->direction);
        }
};

class SpotLight : public PointLight
{
    public:
        glm::vec3 direction;
        float cutOff;
        float outerCutOff;

        SpotLight(
                const glm::vec3& ambient, 
                const glm::vec3& diffuse, 
                const glm::vec3& specular, 
                const glm::vec3& position, 
                const glm::vec3& direction, 
                const float& constant, 
                const float& linear, 
                const float& quadratic, 
                const float& cutOff, 
                const float& outerCutOff, 
                const std::string& prefix = std::string("s")
                ) :
            PointLight(ambient, diffuse, specular, position, constant, linear, quadratic, prefix),
            direction(direction), cutOff(cutOff), outerCutOff(outerCutOff)
        {
        }

        void uploadUniforms(Shader shader, const std::string& lightNum) const
        {
            PointLight::uploadUniforms(shader, lightNum);
            shader.setVec3((this->prefix + "light[" + lightNum + "].direction").c_str(), this->direction);
            shader.setFloat((this->prefix + "light[" + lightNum + "].cutOff").c_str(), this->cutOff);
            shader.setFloat((this->prefix + "light[" + lightNum + "].outerCutOff").c_str(), this->outerCutOff);
        }

        virtual void setDirection(glm::vec3 direction)
        { this->direction = direction; 
        }
};

#endif

