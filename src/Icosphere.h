#pragma once

#define _USE_MATH_DEFINES

#define GLM_ENABLE_EXPERIMENTAL

#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <math.h>
#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "DrawObj.h"
#include "shader.h"

class Icosphere : public IDrawObj {
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

	struct Triangle {
		int i1;
		int i2;
		int i3;
		Triangle(int i1, int i2, int i3) : i1(i1), i2(i2), i3(i3) {}
	};

	Icosphere(std::string name, float radius = 1.0f, int subdivisions = 1, bool smooth = true);
	~Icosphere() {}

	// attributes
	float getRadius() const { return this->radius; }
	int getSubdivisionCount() const { return this->subdivisions; }

	void setRadius(float radius);
	void setSubdivisions(int subdivisions);
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
	void buildIcosphere(float radius, int subdivisions, bool smooth);
	void buildVerticesSmooth();
	void buildVerticesFlat();
	void buildInterleavedVertices();
	void buildIndices();
	glm::vec3 computeFaceNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);

	///<summary>Subdivides all the triangles in the icosphere based on the current shading.</summary>
	void subdivide();
	///<summary>Subdivides a single triangle using smooth shading.</summary>
	void subdivideTriangleSmooth(unsigned int index);
	void subdivideTriangleFlat(unsigned int index);

	void clearData();

	void genVAO();
	void genLineVAO();

	float radius;
	int subdivisions;
	bool smooth;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> lineIndices;
	std::vector<Triangle> triangles; // triangles with indices for each vertex;
	std::unordered_map<glm::vec3, unsigned int> vertexLoc; // index location for each vertex;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<VertexData> interleavedVertices;
	GLuint VAO = 0, VBO, EBO;
	GLuint lineVAO = 0, lineVBO, lineEBO;
};
