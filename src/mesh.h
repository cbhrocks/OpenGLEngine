#pragma once

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
#include "vertexData.h"
#include "DrawObj.h"

class Mesh : public IDrawObj {
    public:

        /*  Functions  */
        // constructor and destructor
		~Mesh() {
			glDeleteVertexArrays(1, &VAO);
		}

		// custom constructors
        Mesh(std::vector<VertexData> vertices, std::vector<GLuint> indices, std::string name, Material* material = new Material()):
			vertices(vertices), indices(indices), IDrawObj(name, material)
        {
            // vertex array object
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            // load data into vertex buffers
			// copy vertex buffer data
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), &vertices.front(), GL_STATIC_DRAW);  

			// copy index buffer data
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices.front(), GL_STATIC_DRAW);
            checkGLError("setupMesh buffers");

            // set the vertex attribute pointers for shader
            glEnableVertexAttribArray(0); // vertex positions
            glEnableVertexAttribArray(1); // vertex normals	
            glEnableVertexAttribArray(2); // vertex UVs (texture coords)
            glEnableVertexAttribArray(3); // vertex tangent
            glEnableVertexAttribArray(4); // vertex biTangent
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, Normal));
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, uv));
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, Tangent));
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, Bitangent));
            
            checkGLError("setupMesh attribs");
			// cleanup
            glBindVertexArray(0);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
        }

        // render the mesh
        void Draw(const Shader& shader, GLuint baseUnit = 0) 
        {
            // bind appropriate textures
			GLuint unit = baseUnit;
			GLuint ambientNr = 0;
			GLuint diffuseNr = 0;
			GLuint specularNr = 0;
			GLuint normalNr = 0;
			GLuint heightNr = 0;
			GLuint reflectNr = 0;
			for (GLuint &texture : this->material->textureAmbient)
			{
                glActiveTexture(GL_TEXTURE0 + unit);
                glBindTexture(GL_TEXTURE_2D, texture);
				shader.setInt("material.texture_ambient" + (ambientNr++ > 0 ? std::to_string(ambientNr) : ""), unit++);
			}
			for (GLuint &texture : this->material->textureDiffuse)
			{
                glActiveTexture(GL_TEXTURE0 + unit);
                glBindTexture(GL_TEXTURE_2D, texture);
				shader.setInt("material.texture_diffuse" + (diffuseNr++ > 0 ? std::to_string(diffuseNr) : ""), unit++);
			}
			for (GLuint &texture : this->material->textureSpecular)
			{
                glActiveTexture(GL_TEXTURE0 + unit);
                glBindTexture(GL_TEXTURE_2D, texture);
				shader.setInt("material.texture_specular" + (specularNr++ > 0 ? std::to_string(specularNr) : ""), unit++);
			}
			for (GLuint &texture : this->material->textureNormal)
			{
                glActiveTexture(GL_TEXTURE0 + unit);
                glBindTexture(GL_TEXTURE_2D, texture);
				shader.setInt("material.texture_normal" + (normalNr++ > 0 ? std::to_string(normalNr) : ""), unit++);
			}
			for (GLuint &texture : this->material->textureReflect)
			{
                glActiveTexture(GL_TEXTURE0 + unit);
                glBindTexture(GL_TEXTURE_2D, texture);
				shader.setInt("material.texture_reflect" + (reflectNr++ > 0 ? std::to_string(reflectNr) : ""), unit++);
			}
			checkGLError("Mesh::Draw bind textures");

			// set material coefficiants
			shader.setVec4("material.ambient", this->material->AmbientColor);
			shader.setVec4("material.diffuse", this->material->DiffuseColor);
			shader.setVec4("material.specular", this->material->SpecularColor);
			shader.setFloat("material.shininess", this->material->Shininess);
			shader.setFloat("material.opacity", this->material->Opacity);
			shader.setFloat("material.reflectivity", this->material->Reflectivity);
			shader.setFloat("material.refractionIndex", this->material->RefractionIndex);
			checkGLError("Mesh::Draw bind constants");

            // draw mesh
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

			for (int i = baseUnit; i <= unit; i++) {
				glActiveTexture(GL_TEXTURE0 + unit);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
            checkGLError("Mesh::Draw draw VAO");
        }

    private:
		// disable copying and delete
		Mesh(Mesh const &) = delete;
		Mesh & operator = (Mesh const &) = delete;

        /*  Render data  */
        GLuint VAO, VBO, EBO;
        std::vector<VertexData> vertices;
        std::vector<GLuint> indices;
};
