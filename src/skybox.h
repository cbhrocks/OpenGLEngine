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

GLuint loadSkyboxTexture(const std::vector<std::string> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (GLuint i = 0; i < faces.size(); i++) {

		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

class Skybox 
{
    public:
        /*  Model Data */
        GLuint VAO;
		GLuint texture;
		glm::vec3 rotation;

		Skybox()
		{}

        Skybox(const std::vector<std::string> faces) : 
			texture(loadSkyboxTexture(faces)),
			rotation(0.0)
        {
			setup();
        }

        // draws the model, and thus all its meshes
        virtual void Draw(const Shader& shader) const
        {
			glDepthMask(GL_FALSE);
            glBindVertexArray(VAO);

			glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);

            glDrawArrays(GL_TRIANGLES, 0, 36);

			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
        }

        //virtual void uploadUniforms(const Shader& shader) const
        //{
		//	shader.setInt("texture1", 0);

        //    glm::mat3 normal = glm::inverseTranspose(glm::mat3(model));

		//	shader.setMat4("M", model);
		//	shader.setMat3("N", normal);
        //    checkGLError("upload uniforms -- matrices");
        //}

		void setRotation(glm::vec3 rotation)
		{
			this->rotation = rotation;
		}

		void addRotation(glm::vec3 rotation)
		{
			this->rotation += rotation;
		}

		GLuint getTexture()
		{
			return this->texture;
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
        }
};

