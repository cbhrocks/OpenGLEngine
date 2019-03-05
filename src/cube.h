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

GLuint loadCubeTexture(const std::string &path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
	Shader shader;

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

class Cube 
{
    public:
        /*  Model Data */
        GLuint VAO;
		GLuint texture;
		GLuint skyboxTexture;
		//Shader* shader;
		glm::vec3 position, scale, rotation;

		Cube()
		{}

        Cube(const std::string &path) : 
			texture(loadCubeTexture(path)),
			rotation(0.0),
			position(0.0),
			scale(1.0)
        {
			setup();
			//this->texture = loadTexture(path);
        }

		void setShader(Shader* shader)
		{
			//this->shader = shader;
		}

        // draws the model, and thus all its meshes
        virtual void Draw() const
        {
            glBindVertexArray(VAO);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->texture);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, this->skyboxTexture);

            glDrawArrays(GL_TRIANGLES, 0, 36);

			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);

			checkGLError("draw cube");
        }

        virtual void uploadUniforms(const Shader& shader) const
        {
			shader.setInt("texture1", 0);
			shader.setInt("skybox", 1);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, this->position);
			//rotate x
			model = glm::rotate(model, glm::radians(this->rotation.x), glm::vec3(1.0, 0.0, 0.0));
			//rotate y
			model = glm::rotate(model, glm::radians(this->rotation.y), glm::vec3(0.0, 1.0, 0.0));
			//rotate z
			model = glm::rotate(model, glm::radians(this->rotation.z), glm::vec3(0.0, 0.0, 1.0));
			model = glm::scale(model, this->scale);
            glm::mat3 normal = glm::inverseTranspose(glm::mat3(model));

			shader.setMat4("M", model);
			shader.setMat3("N", normal);
            checkGLError("upload cube uniforms");
        }

		void setPosition(glm::vec3 position)
		{
			this->position = position;
		}

		void setScale(glm::vec3 scale)
		{
			this->scale = scale;
		}

		void setRotation(glm::vec3 rotation)
		{
			this->rotation = rotation;
		}

		void addRotation(glm::vec3 rotation)
		{
			this->rotation += rotation;
		}

		void setSkybox(GLuint skyboxTexture) 
		{
			this->skyboxTexture = skyboxTexture;
		}

    private:
        GLuint VBO;

        void setup()
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(normalTextureCube), &normalTextureCube, GL_STATIC_DRAW);

			//positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

			//normal coords
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

			//texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

            glBindVertexArray(0);
        }
};

