#pragma once

#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <vector>
#include <math.h>
#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "DrawObj.h"
#include "shader.h"

const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;

// A great thing about structs is that their memory layout is sequential for all its items.
// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
// again translates to 3/2 floats which translates to a byte array.

class Sphere : public IDrawObj {
public:
	struct VertexData {
		// position
		glm::vec3 Position;
		// normal
		glm::vec3 Normal;
		// texCoords
		glm::vec2 TexCoord;
		// tangent
	};

	Sphere(std::string name, float radius = 1.0f, int sectorCount = 36, int stackCount = 18, bool smooth = true);
	~Sphere() {}

	// attributes
	float getRadius() const { return this->radius; }
	int getSectorCount() const { return this->sectorCount; }
	int getStackCount() const { return this->stackCount; }

	void setRadius(float radius);
	void setSectorCount(int sectorCount);
	void setStackCount(int stackCount);
	void setSmooth(bool smooth);

	// matrix data

	// vertex data
	unsigned int getVertexCount() const			{ return this->vertices.size(); }
	unsigned int getNormalCount() const			{ return this->normals.size(); }
	unsigned int getTexCoordCount() const		{ return this->texCoords.size(); }
	unsigned int getIndexCount() const			{ return this->indices.size(); }
	unsigned int getLineIndexCount() const		{ return this->lineIndices.size(); }
	unsigned int getTriangleCount() const		{ return this->indices.size() / 3; }
	unsigned int getVertexSize() const			{ return this->vertices.size() * sizeof(glm::vec3); }
	unsigned int getNormalSize() const			{ return this->normals.size() * sizeof(glm::vec3); }
	unsigned int getTexCoordSize() const		{ return this->texCoords.size() * sizeof(glm::vec2); }
	unsigned int getIndexSize() const			{ return this->indices.size() * sizeof(unsigned int); }
	unsigned int getLineIndexSize() const		{ return this->lineIndices.size() * sizeof(unsigned int); }
	const glm::vec3* getVertices() const		{ return this->vertices.data(); }
	const glm::vec3* getNormals() const			{ return this->normals.data(); }
	const glm::vec2* getTexCoords() const		{ return this->texCoords.data(); }
	const unsigned int* getIndices() const		{ return this->indices.data(); }
	const unsigned int* getLineIndices() const	{ return this->lineIndices.data(); }

	// interleaved vertices
	unsigned int getInterleavedVertexCount() const		{ return this->interleavedVertices.size(); }
	unsigned int getInterleavedVertexSize() const		{ return this->interleavedVertices.size() * sizeof(VertexData); }
	unsigned int getInterleavedStride() const			{ return sizeof(VertexData); }
	const VertexData* getInterleavedVertices() const	{ return this->interleavedVertices.data(); }

	// drawers
	void Draw(const Shader& shader, GLuint baseUnit = 0);
	void drawLines();

private:
	void buildSphere(float radius, int sectorCount, int stackCount, bool smooth);
	void buildVerticesSmooth();
	void buildVerticesFlat();
	void buildInterleavedVertices();
	glm::vec3 computeFaceNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);

	void clearData();

	void genVAO();
	void genLineVAO();

	float radius;
	int sectorCount;
	int stackCount;
	bool smooth;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> lineIndices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<VertexData> interleavedVertices;
	GLuint VAO = 0, VBO, EBO;
	GLuint lineVAO = 0, lineVBO, lineEBO;
};
