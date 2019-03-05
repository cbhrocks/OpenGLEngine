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

using namespace std;

struct VertexData {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    GLuint id;
    string type;
    string path;
};

class Object {
    public:
        /*  Mesh Data  */
        vector<VertexData> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        GLuint VAO;

        /*  Functions  */

        // render the mesh
		virtual void Draw(Shader shader) = 0;

    private:
        /*  Render data  */
        GLuint VBO;

        /*  Functions    */
        // initializes all the buffer objects/arrays
		virtual void setup() = 0;
};
