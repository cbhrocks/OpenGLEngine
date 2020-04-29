#ifndef MESH_H
#define MESH_H

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
#include "TextureManager.h"

// A great thing about structs is that their memory layout is sequential for all its items.
// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
// again translates to 3/2 floats which translates to a byte array.
struct VertexData {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 uv;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

class Mesh {
    public:

        /*  Functions  */
        // constructor and destructor
        Mesh() {
            glGenVertexArrays(1, &VAO);
		}
		~Mesh() {
			glDeleteVertexArrays(1, &VAO);
		}

		// custom constructors
        Mesh(std::vector<VertexData> vertices, std::vector<GLuint> indices, std::map<GLuint, std::string> textures):
			vertices(vertices), indices(indices), textures(textures)
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
        void Draw(const Shader& shader) 
        {
            // bind appropriate textures
			GLuint unit = 0;
			GLuint diffuseNr = 0;
			GLuint specularNr = 0;
			GLuint normalNr = 0;
			GLuint heightNr = 0;
			GLuint reflectNr = 0;
			for (auto &texture : textures)
            {
                // retrieve texture number (the N in diffuse_textureN)
                checkGLError("Draw bind textures");
				std::string uniform_name = texture.second;
                if(texture.second == "diffuse")
					uniform_name += (diffuseNr++ > 0) ? std::to_string(diffuseNr)  : "";
                else if(texture.second == "specular")
					uniform_name += (specularNr++ > 0) ? std::to_string(specularNr)  : "";
                else if(texture.second == "normal")
					uniform_name += (normalNr++ > 0) ? std::to_string(normalNr)  : "";
                else if(texture.second == "height")
					uniform_name += (heightNr++ > 0) ? std::to_string(heightNr)  : "";
				else if (texture.second == "reflect")
					uniform_name += (reflectNr++ > 0) ? std::to_string(reflectNr)  : "";

                // bind the texture and set uniform
                glActiveTexture(GL_TEXTURE0 + unit);
                glBindTexture(GL_TEXTURE_2D, texture.first);
                shader.setInt(("material." + uniform_name).c_str(), unit++);
                checkGLError("Draw bind textures");
            }

            // draw mesh
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            checkGLError("Mesh::Draw draw VAO");
        }

    private:
		// disable copying and delete
		Mesh(Mesh const &) = delete;
		Mesh & operator = (Mesh const &) = delete;

        /*  Render data  */
        GLuint VAO, VBO, EBO;
		std::map<GLuint, std::string> textures;
        std::vector<VertexData> vertices;
        std::vector<GLuint> indices;
};
#endif
