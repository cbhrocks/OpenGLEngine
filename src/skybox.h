#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "stb_image.h"
#include "shader.h"
#include "glHelper.h"
#include "vertexData.h"

class Skybox 
{
    public:
        /*  Model Data */
        GLuint VAO;
		Texture texture;
		glm::vec3 rotation;
		Shader shader;

		Skybox()
		{}

		Skybox(const Texture& ptexture, const Shader& shader) :
			shader(shader),
			texture(ptexture),
			rotation(0.0)
        {
			setup();
        }

        virtual void uploadUniforms()
        {
			this->uploadUniforms(this->shader);
        }

        virtual void uploadUniforms(Shader& shader) const
        {
			shader.Use();
			this->shader.setInt("skybox", 0);
        }

		virtual void Draw()
		{
			this->Draw(this->shader);
		}

        // draws the model, and thus all its meshes
        virtual void Draw(Shader& shader) const
        {
			shader.Use();

			glDepthFunc(GL_LEQUAL);
            glBindVertexArray(VAO);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture.id);

            glDrawArrays(GL_TRIANGLES, 0, 36);

			glBindVertexArray(0);
			glDepthFunc(GL_LESS);
        }

		void setRotation(glm::vec3 rotation)
		{
			this->rotation = rotation;
		}

		void addRotation(glm::vec3 rotation)
		{
			this->rotation += rotation;
		}

		Texture getTexture()
		{
			return this->texture;
		}

		Shader getShader()
		{
			return this->shader;
		}

		void setShader(const Shader& shader)
		{
			this->shader = shader;
		}

    private:
        GLuint VBO;

        void setup()
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertexData), &skyboxVertexData, GL_STATIC_DRAW);

			//positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            glBindVertexArray(0);
			checkGLError("Skybox::setup");
        }
};

