#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"

struct Material {
	glm::vec4 AmbientColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 DiffuseColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 SpecularColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float Shininess = 0.0f;
	float opacity = 1.0f;
	float reflectivity = 0.0f;
	float refractionIndex = 1.0;
	std::vector<GLuint> textureAmbient;
	std::vector<GLuint> textureDiffuse;
	std::vector<GLuint> textureSpecular;
	std::vector<GLuint> textureNormal;
	std::vector<GLuint> textureHeight;
	std::vector<GLuint> textureReflect;
};

class IDrawObj {
    public:
        /*  Mesh Data  */
        // render the mesh
		virtual void Draw(const Shader& shader, GLuint baseUnit = 0) = 0;
};
