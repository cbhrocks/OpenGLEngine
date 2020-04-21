#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "shader.h"
#include "glHelper.h"
#include "TextureManager.h"

class DrawObject {
    public:
		class builder;
        /*  Mesh Data  */
        GLuint VAO;
		GLfloat reflect;
		const Shader* shader;
        std::vector<glm::vec3> Positions;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> TexCoords;
		std::vector<glm::vec3> Tangents;
		std::vector<glm::vec3> Bitangents;
        std::vector<GLuint> Indices;
        std::vector<Texture> Textures;
		Texture* Skybox;
		glm::vec3 position, scale, rotation;
		bool transparent;
		bool highlight;

		DrawObject(
			const Shader* shader,
			std::vector<glm::vec3> positions,
			std::vector<glm::vec3> normals,
			std::vector<glm::vec2> texCoords,
			std::vector<glm::vec3> tangents,
			std::vector<glm::vec3> bitangents,
			std::vector<GLuint> indices,
			std::vector<Texture> textures,
			Texture* skybox,
			bool transparent,
			bool highlight,
			GLfloat reflect
		);

        // render the mesh
		void UploadUniforms();

		virtual void UploadUniforms(const Shader& shader);

		void Draw();

		virtual void Draw(const Shader& shader);

		const Shader* getShader() const;
		void setShader(const Shader* shader);

		bool isTransparent() const;
		void setTransparent(bool transparent);

		bool canHighlight() const;
		void setHighlight(bool highlight);

		GLfloat getReflect() const;
		void setReflective(GLfloat& reflect);

		//positioning functions
		void setPosition(glm::vec3 position);
		glm::vec3 getPosition() const;

		void setScale(glm::vec3 scale);
		glm::vec3 getScale() const;

		void setRotation(glm::vec3 rotation);
		glm::vec3 getRotation() const;
		void addRotation(glm::vec3 rotation);

		void calculateTanAndBitan();

    private:
        /*  Render data  */
        GLuint VBO;

        /*  Functions    */
        // initializes all the buffer objects/arrays
		virtual void setup();
};

class DrawObject::builder {
public:
	builder();

	builder& setPositions(std::vector<glm::vec3> positions);
	builder& setNormals(std::vector<glm::vec3> normals);
	builder& setTexCoords(std::vector<glm::vec2> texCoords);
	builder& setTangents(std::vector<glm::vec3> tangents);
	builder& setBitangents(std::vector<glm::vec3> bitangents);
	builder& setIndices(std::vector<GLuint> indices);
	builder& setTextures(std::vector<Texture> textures);
	builder& setSkybox(Texture* skybox);
	builder& setReflect(GLfloat reflect);
	builder& setTransparent(bool transparent);
	builder& setHighlight(bool highlight);

	DrawObject* build(const Shader* shader) const;

private:
	std::vector<glm::vec3> Positions = {};
	std::vector<glm::vec3> Normals = {};
	std::vector<glm::vec2> TexCoords = {};
	std::vector<glm::vec3> Tangents = {};
	std::vector<glm::vec3> Bitangents = {};
	std::vector<GLuint> Indices = {};
	std::vector<Texture> Textures = {};
	Texture* Skybox = nullptr;
	bool transparent = false;
	bool highlight = false;
	GLfloat reflect = 0;
};
