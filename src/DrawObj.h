#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"

struct Material {
	std::string Name = "default material";
	glm::vec4 AmbientColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	glm::vec4 DiffuseColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	glm::vec4 SpecularColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	float Shininess = 32.0f;
	float Opacity = 1.0f;
	float Reflectivity = 0.0f;
	float RefractionIndex = 1.0;
	std::vector<GLuint> textureAmbient;
	std::vector<GLuint> textureDiffuse;
	std::vector<GLuint> textureSpecular;
	std::vector<GLuint> textureNormal;
	std::vector<GLuint> textureHeight;
	std::vector<GLuint> textureReflect;
};

class IDrawObj {
public:
	IDrawObj(std::string name): name(name), material(new Material()) {}
	IDrawObj(std::string name, Material* material) : name(name), material(material) {}

	/*  Mesh Data  */
	// render the mesh
	virtual void Draw(const Shader& shader, GLuint baseUnit = 0) = 0;

	virtual Material* getMaterial() { return this->material; };
	virtual void setMaterial(Material* material) { this->material = material; };
	virtual std::string getName() { return this->name; };
	virtual void setName(std::string name) { this->name = name; };

protected:
	std::string name;
	Material* material;
};
